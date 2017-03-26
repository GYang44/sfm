#include <cmath>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/cuda.hpp>

#include "frame.hpp"

#ifndef frm_opt_h
#define frm_opt_h


//
int formatKeypoints(std::vector<cv::KeyPoint> & outPoints, const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<std::vector<cv::DMatch>> & inMatch, const std::vector<std::vector<cv::KeyPoint>> & inKeyPoints, const double & distanceThreshold);

int formatKeypoints(std::vector<cv::KeyPoint> & outPoints, const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<std::vector<cv::DMatch>> & inMatch, const std::vector<std::vector<cv::KeyPoint>> & inKeyPoints);

int matchForCamPose(const frame & keyframe, frame & newFrame, const std::vector<std::vector<cv::DMatch>> & inMatch);

void vecKeypointToMatKeypoint(cv::Mat & matKeypoints, const std::vector<cv::KeyPoint> & vecKeypoints);

cv::Mat vecKeypointToMatKeypoint(const std::vector<cv::KeyPoint> & vecKeypoints);

double calKeyPointDist(const cv::KeyPoint & pt1, const cv::KeyPoint & pt2);


#endif
