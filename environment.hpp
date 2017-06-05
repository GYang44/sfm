#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <stdio.h>
#include <iostream>
#include <string>

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

class environment
{
public:
	cv::Mat map1, map2;
	cv::VideoCapture video;
	cv::Mat cameraMatrix;//K
	cv::Mat cameraDistCoeffs;
	environment(std::string inFile);
	void getRemap(const cv::Size2i & imageSize);
	bool grab();
	bool retrieve(cv::Mat & frame, int channel = 0);
};

environment::environment(std::string inFile)
{
	//xml file stroes specification of camera and video path
	std::string cameraSpec, videoPath;

	cv::FileStorage fs(inFile, cv::FileStorage::READ);
	fs["cameraSpec"] >> cameraSpec;
	fs["videoFile"] >> videoPath;
	fs.release();

	//get camera matrix
	fs.open(cameraSpec, cv::FileStorage::READ);
	fs["camera_matrix"] >> cameraMatrix;
	fs["distortion_coefficients"] >> cameraDistCoeffs;

	//get video size
	cv::Size imageSize;
	fs["image_width"] >> imageSize.width;
	fs["image_height"] >> imageSize.height;

	fs.release();

	getRemap(imageSize);

	//get video
	if (videoPath.size() != 0)
	{
		std::cout << "opening file: " << videoPath << std::endl;
		video.open(videoPath);
	}
	else
	{
		std::cout << "opening from camera 0." << std::endl;
		video.open(0);
	}

	return;
}

void environment::getRemap(const cv::Size2i & imageSize)
{
	std::cout << imageSize.width << ' ' << imageSize.height << std::endl;
	//get remap matrix and replace orignial camera matrix and Distortion_Coefficients with caliberated value
	cv::initUndistortRectifyMap(cameraMatrix, cameraDistCoeffs, cv::Mat(),
															getOptimalNewCameraMatrix(cameraMatrix, cameraDistCoeffs, imageSize, 1),
															imageSize, CV_16SC2, map1, map2);

	return;
};

bool environment::grab(){
	return video.grab();
};

bool environment::retrieve(cv::Mat & frame, int channel)
{
	bool tmpBool = video.retrieve(frame, channel);
	cv::remap(frame, frame, map1, map2, cv::INTER_LINEAR);
	return tmpBool;
};

class arucoEnv : public environment
{
public:
	double arucoSize; //size of aruco code in mm
	arucoEnv(std::string inFile);
};

arucoEnv::arucoEnv(std::string inFile):environment(inFile)
{
	cv::FileStorage fs(inFile, cv::FileStorage::READ);
	fs["arucoSize"] >> arucoSize;
	fs.release();
}

class sfmEnv : public environment
{
public:
	std::string samplePath;
	sfmEnv(std::string inFile);
	bool sample(const uint sampleRate=1, const uint sampleNumber = UINT_MAX);//create sample photo to file
};

sfmEnv::sfmEnv(std::string inFile):environment(inFile)
{
	cv::FileStorage fs(inFile, cv::FileStorage::READ);
	fs["samplePath"] >> samplePath;
	fs.release();
	return;
}

bool sfmEnv::sample(const uint sampleRate, const uint sampleNumber)
{
	std::ofstream imgList(samplePath + "IMGList.txt", std::ofstream::out);
	if(!imgList.is_open())
	{
		std::cerr << "can not open: " << samplePath << "IMGList.txt for ouput" << std::endl;
		return false;
	}

	cv::Mat newFrame;

	for(uint framCounter(0), sampleCounter(1);(video.grab() & sampleCounter<= sampleNumber);++framCounter)
	{
		video.retrieve(newFrame);
		if (framCounter % sampleRate == 1)
		{
			imgList << "IMG" << std::to_string(framCounter) << ".png" << std::endl;
			//imgList << samplePath << "IMG" << std::to_string(framCounter) << ".png" << std::endl;
			cv::imwrite(samplePath + "IMG" + std::to_string(framCounter) + ".png", newFrame);
			sampleCounter++;
		}
	}
	imgList.close();
	video.release();
	return true;
}

#endif
