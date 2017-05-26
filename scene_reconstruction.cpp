#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <stdlib.h>

#include <opencv2/sfm.hpp>
#include <opencv2/viz.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

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
  system("exec rm ./tmp/*");

  //frames of keypoints and descriptors for entire video
  std::vector<std::string> images;

  printMat<double>(workEnv.cameraMatrix);

  cv::Mat newImgFrame;

  uint frameCounter(0);
  cv::Matx33d K = (workEnv.cameraMatrix.at<double>(0,0), 0, workEnv.cameraMatrix.at<double>(0,2), 0, workEnv.cameraMatrix.at<double>(1,1), workEnv.cameraMatrix.at<double>(1,2), 0, 0, 1);


  while(workEnv.video.grab())
  {
    //convert new frame to gray scale on GPU
    workEnv.video.retrieve(newImgFrame);

    //rectify camera distortion
    cv::remap(newImgFrame, newImgFrame, workEnv.map1, workEnv.map2, cv::INTER_LINEAR);

    frameCounter++;

    if ((frameCounter % 5) == 0)
    {
      //store image
      std::stringstream imageOutFile;
      imageOutFile << "/home/gyang/TemporaryDoc/ProjectDrone/sfm/build/tmp/frame_" << std::setw(6) << std::setfill( '0' ) << (int)frameCounter/5 << "_0.png" ;

      images.push_back(imageOutFile.str());
      cv::cvtColor(newImgFrame, newImgFrame, 7);
      cv::imwrite(images.back(), newImgFrame);
    }
/*
    if ((frameCounter % 25) == 0)
    {
      //reconstruct
      cv::Mat points3d;
      std::vector<cv::Mat> Rs, Ts;
      cv::sfm::reconstruct(images, Rs, Ts, K, points3d, true);
      for(uint i(0); i < Rs.size(); i++)
      {
        camera.updateRT(Rs[i], Ts[i]);
        camera.updateWrP(camera);
      }
      images.clear();
      std::cout << "finished reconstruction for frame " << std::to_string(frameCounter) << std::endl;
      std::cout << "press any key to countinue" << std::endl;
      //cv::waitKey();
    }
*/
    //cv::imshow("newFrame", newImgFrame);
    if (cv::waitKey(10) == 27) break;
  }

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

  return 0;
}
