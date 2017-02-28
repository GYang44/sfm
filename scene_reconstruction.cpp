#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>

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

#include "hc.hpp"

/*
int getdir(const std::string _filename, std::vector<std::string> &files)
{
    std::ifstream myfile(_filename.c_str());
    if (!myfile.is_open()) {
        std::cout << "Unable to read file: " << _filename << std::endl;
        exit(0);
    } else {;
        size_t found = _filename.find_last_of("/\\");
        std::string line_str, path_to_file = _filename.substr(0, found);
        while ( getline(myfile, line_str) )
            files.push_back(path_to_file+std::string("/")+line_str);
    }
    return 1;
}
*/

int main(int argc, char* argv[])
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

    while(workEnv.video.grab())
    {
        workEnv.video.retrieve(newFrame);
        newFrameGpu.upload(newFrame);
        cv::cuda::cvtColor(newFrameGpu, newFrameGpuGray, CV_RGB2GRAY);

        //detect keypoints and compute descriptors
        std::vector<cv::KeyPoint> frameKeypoints;
        cv::cuda::GpuMat frameDescriptors;
        CudaDetector -> detectAndCompute(newFrameGpuGray, cv::cuda::GpuMat(), frameKeypoints, frameDescriptors);

        //
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
            cv::drawMatches(newFrame, keyPoints[keyPoints.size()-1], oldFrame, keyPoints[keyPoints.size()-2], newMatchs, drawFrame);
            newFrameGpu.upload(drawFrame);
            cv::imshow("cudaFeature", newFrameGpu);
        }

        if (cv::waitKey(10) == 27) break;
        oldFrame = newFrame;
    }

    return 0;
}
