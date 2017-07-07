#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>
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

#include <opencv2/sfm.hpp>
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
float xRot = 30.0f;
float yRot = 70.0f;
float zRot = 0.0f;

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
  cv::cuda::setDevice(0);

  //create feature detector
  cv::cuda::SURF_CUDA CudaDetector(2000);

  //tmp variable for frame handling
  cv::cuda::GpuMat newFrameGpu, newFrameGpuGray;
  cv::Mat *newFrameImg = new cv::Mat;
  cv::Mat *oldFrameImg = new cv::Mat;

  //feature Detector
  cv::Ptr<cv::cuda::DescriptorMatcher> matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_L2);

  cv::Mat drawFrame;

  cv::cuda::GpuMat *oldFrameDescriptorsGpu = new cv::cuda::GpuMat;
  cv::cuda::GpuMat *newFrameDescriptorsGpu = new cv::cuda::GpuMat;
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
    if ((frameCount % 2 == 1))
    {
      newFrameGpu.upload(*newFrameImg);
      cv::cuda::cvtColor(newFrameGpu, newFrameGpuGray, CV_RGB2GRAY);

      //detect keypoints and compute descriptors
      CudaDetector(newFrameGpuGray, cv::cuda::GpuMat(), *newFrameKeypoints, *newFrameDescriptorsGpu);

      //matches
      cv::cuda::GpuMat newMatchesGpu;
      std::vector<std::vector<cv::DMatch>> newMatches;

      if (!(oldFrameImg -> empty()))
      {
        //find matches, newFrame is query, oldFrame is train
        matcher -> knnMatchAsync(*newFrameDescriptorsGpu, *oldFrameDescriptorsGpu, newMatchesGpu, 1);
        matcher -> knnMatchConvert(newMatchesGpu, newMatches, false);

        //remove outliner from matchvector
        int resudual = rmOutliner(*newFrameKeypoints, *oldFrameKeypoints, newMatches, 40);

        if (resudual > 7)
        {
          //get keypoints that match
          std::vector<cv::KeyPoint> queryVec, trainVec;
          getMatchedKeypoints(*newFrameKeypoints, *oldFrameKeypoints, newMatches, queryVec, trainVec);

          //format keypoints
          cv::Mat queryMat, trainMat;
          vecKeypointToMatKeypoint(queryMat, queryVec);
          vecKeypointToMatKeypoint(trainMat, trainVec);

          //get fundamental matrix
          cv::Mat F;
          //cv::sfm::normalizedEightPointSolver(trainMat, queryMat, F);
          const double max_error = 3, outliers_probability = 0.01;
          std::vector<int> inliers;
          //X_2 = R * X_1 + T
          //x_2^T * F * x_1 = 0
          //query ~ newFrame ~ x1, train ~ oldFrame ~ x2
          cv::sfm::fundamentalFromCorrespondences8PointRobust(queryMat, trainMat, max_error, F, inliers, outliers_probability);

          //get essential matrix
          cv::Mat E;
          cv::sfm::essentialFromFundamental(F, workEnv.cameraMatrix, workEnv.cameraMatrix, E);

          //get R T
          std::vector<cv::Mat> Rs, ts;
          cv::sfm::motionFromEssential(E, Rs, ts);
          int solutionNum = cv::sfm::motionFromEssentialChooseSolution(Rs, ts, workEnv.cameraMatrix, trainMat.colRange(cv::Range(inliers[0],inliers[0]+1)), workEnv.cameraMatrix, queryMat.colRange(cv::Range(inliers[0],inliers[0]+1)));

          if(solutionNum >= 0)
          {
            //tweaked fo robust estimation
            camera.updateRT(Rs[solutionNum],ts[solutionNum]);
            camera.calWrP();
          }
          else
          {
            std::cout << "no solution found" << std::endl;
          }
        }
        else
        {
          std::cout << "not enough matches found after remove outliner" << std::endl;
        }
        //visualize matches
        cv::drawMatches(*newFrameImg, *newFrameKeypoints, *oldFrameImg, *oldFrameKeypoints, newMatches, drawFrame);
        cv::imshow("matches", drawFrame);
        char inChar = cv::waitKey(10);
        if (inChar == 27) return;
      }
      //swap image keypoints and descriptor for previous frame
      swapPointer(oldFrameImg, newFrameImg);
      swapPointer(oldFrameKeypoints, newFrameKeypoints);
      swapPointer(oldFrameDescriptorsGpu, newFrameDescriptorsGpu);
    }
  }
  if (newFrameImg != NULL) delete newFrameImg;
  if (oldFrameImg != NULL) delete oldFrameImg;
  if (newFrameDescriptorsGpu != NULL) delete newFrameDescriptorsGpu;
  if (oldFrameDescriptorsGpu != NULL) delete oldFrameDescriptorsGpu;
  if (newFrameKeypoints != NULL) delete newFrameKeypoints;
  if (oldFrameKeypoints != NULL) delete oldFrameKeypoints;
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
