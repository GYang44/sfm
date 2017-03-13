#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
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

//camera view point (angle)
object3D camera;

//global variable for view angle
float xRot = -70.0f;
float yRot = 0.0f;
float zRot = 30.0f;


template<typename Tp>
Tp printMat(const cv::Mat & mat)
{
    std::cout << std::endl;
    for (int row(0); row < mat.size().height; row++)
    {
        for (int col(0); col < mat.size().width; col++)
        {
            std::cout << mat.at<Tp>(row,col) << ' ';
        }
        std::cout << std::endl;
    }
    return 0;
}

double calKeyPointDist(const cv::KeyPoint & pt1, const cv::KeyPoint & pt2)
{
    return sqrt(pow((pt1.pt.x - pt2.pt.x),2) + pow((pt1.pt.y - pt2.pt.y),2));
}

bool matchForCamPose(std::vector<cv::Mat> & outPoints, const std::vector<std::vector<cv::DMatch>> & inMatch, const std::vector<std::vector<cv::KeyPoint>> & inKeyPoints, const double & distanceThreshold)
//create array of corresponding points for camera pose estimation
{
    cv::Mat pointSet1, pointSet2;
    //check distance
    for (int inMatchCt(0); inMatchCt < inMatch.size(); inMatchCt++)
    {
        int keyPointNum = inMatch[inMatchCt][0].queryIdx;
        if ( calKeyPointDist(inKeyPoints[inKeyPoints.size() - 2][inMatchCt], inKeyPoints[inKeyPoints.size() - 1][keyPointNum]) <= distanceThreshold)
        {
            pointSet1.push_back(cv::Mat(cv::Vec<double, 2>(inKeyPoints[inKeyPoints.size() - 2][inMatchCt].pt.x, inKeyPoints[inKeyPoints.size() - 2][inMatchCt].pt.y)).t());
            pointSet2.push_back(cv::Mat(cv::Vec<double, 2>(inKeyPoints[inKeyPoints.size() - 1][keyPointNum].pt.x, inKeyPoints[inKeyPoints.size() - 1][keyPointNum].pt.y)).t()); //store matched points from current frame
        }
    }
    if (pointSet1.size().height > 7 )
    {
        outPoints.push_back(pointSet1.t());
        outPoints.push_back(pointSet2.t());
        return true;
    }
    else
    {
        return false;
    }
}

bool matchForCamPose(std::vector<cv::Mat> & outPoints, const std::vector<std::vector<cv::DMatch>> & inMatch, const std::vector<std::vector<cv::KeyPoint>> & inKeyPoints)
//create keypoints for camera pose estimation
{
    return matchForCamPose(outPoints, inMatch, inKeyPoints, 20);
}

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
    cv::Ptr<cv::cuda::ORB> CudaDetector = cv::cuda::ORB::create(500, 1.2f, 8, 31, 0, 2, 0, 31, 20, true);

    //tmp variable for frame handling
    cv::cuda::GpuMat newFrameGpu, newFrameGpuGray;
    cv::Mat newFrame;

    //feature descriptors for each frame
    cv::cuda::GpuMat newFrameFeature;
    //keypoints for entire video
    std::vector<std::vector<cv::KeyPoint>> keyPoints;
    //feature descriptors for entire video
    std::vector<cv::cuda::GpuMat> descriptors;
    //all matches
    std::vector<std::vector<std::vector<cv::DMatch>>> matchs;

    cv::Ptr<cv::cuda::DescriptorMatcher> matcher = cv::cuda::DescriptorMatcher::createBFMatcher(cv::NORM_HAMMING);

    cv::Mat oldFrame, drawFrame;

    double distThreshold = sqrt(pow(640, 2) + pow(480, 2));

    //matched keypoints for camera pose estimation
    std::vector<cv::Mat> keyMatch;

    while(workEnv.video.grab())
    {
        workEnv.video.retrieve(newFrame);
        newFrameGpu.upload(newFrame);
        cv::cuda::cvtColor(newFrameGpu, newFrameGpuGray, CV_RGB2GRAY);

        //detect keypoints and compute descriptors
        std::vector<cv::KeyPoint> frameKeypoints;
        cv::cuda::GpuMat frameDescriptors;
        CudaDetector -> detectAndCompute(newFrameGpuGray, cv::cuda::GpuMat(), frameKeypoints, frameDescriptors);

        //store descriptors and keypoints
        descriptors.push_back(frameDescriptors);
        keyPoints.push_back(frameKeypoints);

        //matches in from the new frame
        cv::cuda::GpuMat newMatchsGpu;
        std::vector<std::vector<cv::DMatch>> newMatchs;
        if (descriptors.size() > 1)
        {
            //find matches
            matcher -> knnMatchAsync(descriptors[descriptors.size() - 1], descriptors[descriptors.size() - 2], newMatchsGpu, 1);
            matcher -> knnMatchConvert(newMatchsGpu, newMatchs, false);

            matchs.push_back(newMatchs);

            if (matchForCamPose(keyMatch, newMatchs, keyPoints))
            {
                cv::Mat F;
                cv::sfm::normalizedEightPointSolver(keyMatch[keyMatch.size() - 1], keyMatch[keyMatch.size() - 2], F);
                cv::Mat E;
                cv::sfm::essentialFromFundamental(F, workEnv.cameraMatrix, workEnv.cameraMatrix, E);
                std::vector<cv::Mat> Rs,ts;//candidates for correct Rotation and translation matrices
                cv::sfm::motionFromEssential(E, Rs, ts);
                int solutionNum = cv::sfm::motionFromEssentialChooseSolution(Rs, ts, workEnv.cameraMatrix, keyMatch[keyMatch.size() - 1](cv::Range(0,2),cv::Range(0,1)), workEnv.cameraMatrix, keyMatch[keyMatch.size() - 2](cv::Range(0,2),cv::Range(0,1)));

                if(solutionNum >= 0)
                {
                    camera.updateRT(Rs[solutionNum],ts[solutionNum]);
                    camera.updateWrP(camera);
                }
                else
                {
                    std::cout << "no soluion found" << std::endl;
                }
            }
            else
            {

                descriptors.erase(descriptors.end());
                std::cout << "failed to find enough points, last frame erased" << std::endl;
            }

            cv::drawMatches(newFrame, keyPoints[keyPoints.size() - 1], oldFrame, keyPoints[keyPoints.size() - 2], newMatchs, drawFrame);
            newFrameGpu.upload(drawFrame);
            cv::imshow("cudaFeature", newFrameGpu);
        }

        if (cv::waitKey(30) == 27) break;
        oldFrame = newFrame;
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
