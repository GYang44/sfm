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

    if(cv::cuda::getCudaEnabledDeviceCount())
    {
        cv::cuda::printCudaDeviceInfo(0);
    }

    cv::namedWindow("cudaFeature", cv::WINDOW_OPENGL);
    //cv::cuda::setGlDevice();

    cv::cuda::setDevice(0);

    environment workEnv("../setting.xml");

    cv::Ptr<cv::cuda::ORB> CudaDetector = cv::cuda::ORB::create(500, 1.2f, 8, 31, 0, 2, 0, 31, 20, true);
    cv::cuda::GpuMat newFrameGpu, newFrameGpuGray;
    cv::Mat newFrame;
    cv::cuda::GpuMat newFrameFeature;
    std::vector<std::vector<cv::KeyPoint>> KeyPoint;
    std::vector<cv::Mat> descriptor;
    while(workEnv.video.grab())
    {
        workEnv.video.retrieve(newFrame);
        newFrameGpu.upload(newFrame);
        cv::cuda::cvtColor(newFrameGpu, newFrameGpuGray, CV_RGB2GRAY);
        std::vector<cv::KeyPoint> frameKeypoints;
        cv::cuda::GpuMat frameDescriptor;

        CudaDetector -> detectAndCompute(newFrameGpuGray, cv::cuda::GpuMat(), frameKeypoints, frameDescriptor);
        cv::drawKeypoints( newFrame, frameKeypoints, newFrame, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
        newFrameGpu.upload(newFrame);
        cv::imshow("cudaFeature", newFrameGpu);
        cv::waitKey(10);
    }


    return 0;
}
