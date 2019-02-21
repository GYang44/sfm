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
	cv::VideoCapture inVideo;
	cv::VideoWriter outVideo;
	cv::Mat cameraMatrix;//K
	cv::Mat cameraDistCoeffs;
	cv::Point2d principlePoint;
	double focalLength;

	environment(std::string inFile);
	void getRemap(const cv::Size2i & imageSize);
	bool grab();
	bool retrieve(cv::Mat & frame, int channel = 0);
	bool store(const cv::Mat & frame);
	void updateFocalPc();
};

environment::environment(std::string inFile):
principlePoint(0,0), focalLength(0)
{
	//xml file stroes specification of camera and video path
	std::string cameraSpec, inVideoPath, outVideoPath;

	cv::FileStorage fs(inFile, cv::FileStorage::READ);
	fs["cameraSpec"] >> cameraSpec;
	fs["inputVideoFile"] >> inVideoPath;
	fs["outputVideoFile"] >> outVideoPath;
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

	//get input video
	if (inVideoPath.size() != 0)
	{
		std::cout << "opening file: " << inVideoPath << std::endl;
		inVideo.open(inVideoPath);
	}
	else
	{
		std::cout << "opening from camera 0." << std::endl;
		inVideo.open(0);
	}

	//prepare output video
	if (outVideoPath.size() != 0)
	{
		//todo remove status check
		int codec = outVideo.fourcc('M', 'J', 'P', 'G');
		outVideo.open(outVideoPath, codec, 25.0, imageSize);
	}
	else
	{
		outVideo.release();
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
	updateFocalPc();
	return;
};

bool environment::grab(){
	return inVideo.grab();
};

bool environment::retrieve(cv::Mat & frame, int channel)
{
	bool tmpBool = inVideo.retrieve(frame, channel);
	cv::remap(frame, frame, map1, map2, cv::INTER_LINEAR);
	return tmpBool;
};

bool environment::store(const cv::Mat & frame)
{
	if (outVideo.isOpened())
	{
		outVideo.write(frame);
		return true;
	}
	else
	{
		return false;
	};
};

void environment::updateFocalPc()
{
	principlePoint.x = cameraMatrix.at<double>(0,2);
	principlePoint.y = cameraMatrix.at<double>(1,2);
	focalLength = cameraMatrix.at<double>(1,1);
	return;
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
	if (samplePath.size() == 0)
	{
		std::cout << "no sample path specified" << std::endl;
	}
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

	for(uint framCounter(0), sampleCounter(1);( inVideo.grab() & (sampleCounter <= sampleNumber));++framCounter)
	{
		inVideo.retrieve(newFrame);
		if (framCounter % sampleRate == 1)
		{
			imgList << "IMG" << std::to_string(framCounter) << ".png" << std::endl;
			//imgList << samplePath << "IMG" << std::to_string(framCounter) << ".png" << std::endl;
			cv::imwrite(samplePath + "IMG" + std::to_string(framCounter) + ".png", newFrame);
			sampleCounter++;
		}
	}
	imgList.close();
	inVideo.release();
	return true;
}


#endif
