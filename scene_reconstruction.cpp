#define CERES_FOUND true

#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/sfm.hpp>
#include <opencv2/viz.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>

#include <opencv2/core/cuda.hpp>

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

    for(int feature = cv::cuda::FEATURE_SET_COMPUTE_10; feature != cv::cuda::DYNAMIC_PARALLELISM; feature++)
    {
        if (cv::cuda::deviceSupports( (cv::cuda::FeatureSet)feature))
        {
            std::cout << "yes" << std::endl;
        }
        else
        {
            std::cout << "no" << std::endl;
        }
    }

    cv::cuda::setDevice(0);

    environment workEnv("../setting.xml");

    cv::Ptr<cv::FastFeatureDetector> CudaDetector = cv::cuda::FastFeatureDetector::create();

    cv::Mat newFrame;
    cv::cuda::GpuMat newFrameFeature;
    cv::cuda::GpuMat frameTest(100,100, NULL);
    while(workEnv.video.grab())
    {
        workEnv.video.retrieve(newFrame);
        std::vector<cv::KeyPoint> Keypoints;
        CudaDetector -> detect(frameTest, Keypoints);
        //drawKeypoints( newFrame, Keypoints, newFrameFeature, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT );
        //cv::imshow("cudaFeature", newFrameFeature);
        cv::waitKey(10);
    }


    return 0;
}
