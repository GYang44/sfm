#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>
#include <thread>

#include <opencv2/sfm.hpp>
#include <opencv2/viz.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <opencv2/sfm.hpp>

#include "environment.hpp"
#include "glVisualizer.hpp"
#include "object3D.hpp"
#include "frame.hpp"
#include "frameOperation.hpp"
#include "debugger.hpp"

//camera view point (angle)
object3D camera;

//global variable for view angle
float xRot = -70.0f;
float yRot = 0.0f;
float zRot = 30.0f;

//void camPoseFromVideo(environment & workEnv)
void camPoseFromVideo()
{
  // Read input parameters
  //sfmEnviroment wkEnv(std::string("../setting.xml"));
  environment workEnv("../setting.xml");
  //create new window set device
  cv::namedWindow("cudaFeature", cv::WINDOW_OPENGL);
  cv::cuda::setDevice(0);

  //create feature detector
  cv::cuda::SURF_CUDA CudaDetector(0.5);

  //tmp variable for frame handling
  cv::cuda::GpuMat newImgFrameGpu, newImgFrameGpuGray;
  cv::Mat newImgFrame;

  //frames of keypoints and descriptors for entire video
  std::vector<frame> frames;
  std::vector<cv::Mat> segmentFrames;

  //feature Detector
  cv::Ptr<cv::cuda::DescriptorMatcher> matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_L2);

  cv::Mat oldFrame, drawFrame;

  while(workEnv.grab())
  {
    //convert new frame to gray scale on GPU
    workEnv.retrieve(newImgFrame);
    newImgFrameGpu.upload(newImgFrame);
    cv::cuda::cvtColor(newImgFrameGpu, newImgFrameGpuGray, CV_RGB2GRAY);

    //detect keypoints and compute descriptors
    std::vector<cv::KeyPoint> frameKeypoints;
    cv::cuda::GpuMat frameDescriptorsGpu;
    CudaDetector(newImgFrameGpuGray, cv::cuda::GpuMat(), frameKeypoints, frameDescriptorsGpu);

    //create new regular frame with detected keypoints
    frames.push_back(frame(frameKeypoints));

    //matches in from the new frame
    cv::cuda::GpuMat newMatchsGpu;

    std::vector<std::vector<cv::DMatch>> newMatchs;

    // check if its the first frame;
    if (frames.size() > 1 )
    {
      //specify key frame for current frame
      frames[frames.size() - 1].keyframe = frames[frames.size() - 2].keyframe;

      //find matches, in newMatchsGpu or newMatchs, the first element contains the candidates in second descriptors matching to first element in first descriptor
      matcher -> knnMatchAsync(frameDescriptorsGpu, frames[frames[frames.size() - 2].keyframe].descriptors, newMatchsGpu, 1);
      matcher -> knnMatchConvert(newMatchsGpu, newMatchs, false);

      //remove outliner
      int matchedKeypoint = rmOutliner(frames[frames[frames.size() - 2].keyframe].keypoint, frames[frames.size() - 1].keypoint, newMatchs);

      //matched keypoint after remove outliner
      //int matchedKeypoint = matchForCamPose(frames[frames[frames.size() - 2].keyframe], frames[frames.size() - 1], newMatchs);

      // if not add new key frame
      if (matchedKeypoint <= 10 | segmentFrames.size() >= 5)
      {

        if (matchedKeypoint < 7)
        {
          std::cout << "failed to find enough points, last frame erased" << std::endl;
          frames.pop_back();
        }
        else
        {
          /*
          std::cout << "start a new local reconstruction" << std::endl;
          // add new keyframe and reconstruct

          std::vector<cv::Mat> Rs, Ts, point3d;
          cv::sfm::reconstruct(segmentFrames, Rs, Ts, workEnv.cameraMatrix, point3d, true);
          for (int i(0); i < Rs.size(); i++)
          {
            camera.updateRT(Rs[i], Ts[i]);
            camera.updateWrP(camera);
          }
          */
          // reset segmentFrames after reconstruction
          segmentFrames.clear();
          segmentFrames.push_back(frames[frames.size() - 1].keypointAsKeyframe);

        }

      }
      else
      {
        // add keypoints of a regular frame to the segment
        segmentFrames.push_back(frames[frames.size() - 1].keypointToKeyframe);
        std::cout << "just another frame" << std::endl;
      }
      if (!newMatchs.empty())
        cv::drawMatches(newImgFrame, frames[frames.size() - 1].keypoint, oldFrame, frames[frames.size() - 2].keypoint, newMatchs, drawFrame);
      newImgFrameGpu.upload(drawFrame);
      cv::imshow("cudaFeature", newImgFrameGpu);

    }
    // create keyframe if it's the first frame
    else
    {
      //create first frame as key frame
      frameDescriptorsGpu.copyTo(frames[0].descriptors);
      frames[0].keyframe = 0;
      oldFrame = newImgFrame;
    }
    char inChar = cv::waitKey();
    if (inChar == 27) break;

  }
  return;
}

int main(int argc, char* argv[])
{
  //object3D camera;
  camera.iniWr();
  camera.isInitialed = true;

  std::thread camEst (camPoseFromVideo);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (1280, 720);
  glutInitWindowPosition (0,0);
  glutCreateWindow( "Live Show" );
  glutDisplayFunc( renderScene );
  glutReshapeFunc( changeSize );
	glutKeyboardFunc( viewControl );
  setupRC();
  glutMainLoop();
  camEst.join();
  return 0;
}
