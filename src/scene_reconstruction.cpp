#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <thread>

#include <opencv2/sfm.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudafeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <opencv2/sfm/robust.hpp>

#include "environment.hpp"
#include "glVisualizer.hpp"
#include "object3D.hpp"
#include "frame.hpp"
#include "frameOperation.hpp"
#include "debugger.hpp"

//camera view point (angle)
CameraObj camera;

//global variable for view angle
float xRot = 110.0f;
float yRot = 0.0f;
float zRot = 110.0f;

template<typename Tp_>
void swapPointer(Tp_ *& ptLeft, Tp_ *& ptRight)
{
  Tp_ * tmp = ptLeft;
  ptLeft = ptRight;
  ptRight = tmp;
  return;
}

//void camPoseFromVideo(environment & workEnv)
void camPoseFromVideo()
{
  // Read input parameters
  //sfmEnviroment wkEnv(std::string("../setting.xml"));
  environment workEnv("../setting.xml");
  //create new window set device
  //cv::cuda::setDevice(0);

  //create feature detector
  //cv::cuda::SURF_CUDA CudaDetector(2000);
  cv::Ptr<cv::xfeatures2d::SURF>CpuDetector = cv::xfeatures2d::SURF::create(2000);

  //tmp variable for frame handling
  //cv::cuda::GpuMat newFrameGpu, newFrameGpuGray;
  cv::Mat newFrameCpu, newFrameCpuGray;
  cv::Mat *newFrameImg = new cv::Mat;
  cv::Mat *oldFrameImg = new cv::Mat;

  //feature Detector
  //cv::Ptr<cv::cuda::DescriptorMatcher> matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_L2);
  cv::Ptr<cv::BFMatcher> matcher(new cv::BFMatcher(cv::NORM_L2));

  cv::Mat drawFrame;

  //cv::cuda::GpuMat *oldFrameDescriptorsGpu = new cv::cuda::GpuMat;
  //cv::cuda::GpuMat *newFrameDescriptorsGpu = new cv::cuda::GpuMat;
  cv::Mat *oldFrameDescriptorCpu = new cv::Mat();
  cv::Mat *newFrameDescriptorCpu = new cv::Mat();
  std::vector<cv::KeyPoint> *newFrameKeypoints = new std::vector<cv::KeyPoint>;
  std::vector<cv::KeyPoint> *oldFrameKeypoints = new std::vector<cv::KeyPoint>;

  int frameCount(0);

  while(workEnv.grab())
  {
    frameCount++;
    //convert new frame to gray scale on GPU
    workEnv.retrieve(*newFrameImg);

    // check if its the first frame;
    //if ((frameCount % 10 == 1))
    if ((frameCount % 6 == 1))
    {
      //newFrameGpu.upload(*newFrameImg);
      newFrameCpu = *newFrameImg;
      //cv::cuda::cvtColor(newFrameGpu, newFrameGpuGray, CV_RGB2GRAY);
      cv::cvtColor(newFrameCpu, newFrameCpuGray,CV_RGB2GRAY);

      //detect keypoints and compute descriptors
      //CudaDetector(newFrameGpuGray, cv::cuda::GpuMat(), *newFrameKeypoints, *newFrameDescriptorsGpu);
      CpuDetector -> detect(newFrameCpuGray, *newFrameKeypoints);
      CpuDetector -> compute(newFrameCpuGray, *newFrameKeypoints, *newFrameDescriptorCpu);

      //matches
      //cv::cuda::GpuMat newMatchesGpu;
      std::vector<std::vector<cv::DMatch>> newMatches;

      if (!(oldFrameImg -> empty()))
      {

        //find matches, newFrame is query, oldFrame is train
        //matcher -> knnMatchAsync(*newFrameDescriptorsGpu, *oldFrameDescriptorsGpu, newMatchesGpu, 1);
        //matcher -> knnMatchConvert(newMatchesGpu, newMatches, false);
        matcher -> knnMatch(*newFrameDescriptorCpu, *oldFrameDescriptorCpu, newMatches, 1);

        //remove outliner from matchvector
        int residual = rmOutliner(*newFrameKeypoints, *oldFrameKeypoints, newMatches, 40);

        drawFrame = *newFrameImg;

        if (residual > 7)
        {
          //get keypoints that match
          std::vector<cv::KeyPoint> queryVec, trainVec;
          getMatchedKeypoints(*newFrameKeypoints, *oldFrameKeypoints, newMatches, queryVec, trainVec);

          //format keypoints
          cv::Mat queryMat, trainMat;
          vecKeypointToMatKeypoint(queryMat, queryVec);
          vecKeypointToMatKeypoint(trainMat, trainVec);
          cv::transpose(queryMat,queryMat);
          cv::transpose(trainMat,trainMat);

          //get fundamental matrix
          //cv::Mat F, inLiers;
          //cv::sfm::normalizedEightPointSolver(trainMat, queryMat, F);

          //get essential matrix
          cv::Mat E;
          //cv::sfm::essentialFromFundamental(F, workEnv.cameraMatrix, workEnv.cameraMatrix, E);
          E = cv::findEssentialMat(queryMat, trainMat, workEnv.focalLength, workEnv.principlePoint, cv::RANSAC, 0.99, 20.0);

          //get R T
          //std::vector<cv::Mat> Rs, ts;
          //cv::sfm::motionFromEssential(E, Rs, ts);
          //int solutionNum = cv::sfm::motionFromEssentialChooseSolution(Rs, ts, workEnv.cameraMatrix, trainMat(cv::Range(0,2),cv::Range(0,1)), workEnv.cameraMatrix, queryMat(cv::Range(0,2),cv::Range(0,1)));
          cv::Mat R,t;
          cv::recoverPose(E, queryMat, trainMat, R, t, workEnv.focalLength, workEnv.principlePoint);

          //if(solutionNum >= 0)
          //{
          //  camera.updateRT(Rs[solutionNum],ts[solutionNum]);
          //  camera.calWrP();
          //}
          //else
          //{
          //  std::cout << "no solution found" << std::endl;
          //}
          //draw movement of the featurepoints
          camera.updateRT(R,t);
          camera.calWrP();

          trackMatches(drawFrame, queryVec, trainVec);
        }
        else
        {
          std::cout << "not enough matches found after remove outliner" << std::endl;
        }
        cv::imshow("track", drawFrame);

        //output video
        workEnv.store(drawFrame);
        //output location
        std::cout << camera.p(0,0) << ',' << camera.p(1,0) << ',' << camera.p(2,0) << std::endl;
        char inChar = cv::waitKey(1);
        if (inChar == 27) return;
      }
      //swap image keypoints and descriptor for previous frame
      swapPointer(oldFrameImg, newFrameImg);
      swapPointer(oldFrameKeypoints, newFrameKeypoints);
      //swapPointer(oldFrameDescriptorsGpu, newFrameDescriptorsGpu);
      swapPointer(oldFrameDescriptorCpu, newFrameDescriptorCpu);
    }
  }
  if (newFrameImg != NULL) delete newFrameImg;
  if (oldFrameImg != NULL) delete oldFrameImg;
  //if (newFrameDescriptorsGpu != NULL) delete newFrameDescriptorsGpu;
  //if (oldFrameDescriptorsGpu != NULL) delete oldFrameDescriptorsGpu;
  if (newFrameDescriptorCpu != NULL) delete newFrameDescriptorCpu;
  if (oldFrameDescriptorCpu != NULL) delete oldFrameDescriptorCpu;
  if (newFrameKeypoints != NULL) delete newFrameKeypoints;
  if (oldFrameKeypoints != NULL) delete oldFrameKeypoints;
  return;
}

int main(int argc, char* argv[])
{
  //object3D camera;
  camera.iniWr(M_PI/2,M_PI,0);
  camera.isInitialed = true;

  std::thread camEst (camPoseFromVideo);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize (1280, 720);
  glutInitWindowPosition (0,0);
  glutCreateWindow( "Live Plot" );
  glutDisplayFunc( renderScene );
  glutReshapeFunc( changeSize );
	glutKeyboardFunc( viewControl );
  setupRC();
  glutMainLoop();
  camEst.join();
  return 0;
}
