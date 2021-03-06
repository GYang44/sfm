#include <cmath>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/cuda.hpp>

#include "frame.hpp"
#include "debugger.hpp"

#ifndef frm_opt_h
#define frm_opt_h

//get aligned keypoints from DMatch information
int formatKeypoints(std::vector<cv::KeyPoint> & outPoints, const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<std::vector<cv::DMatch>> & inMatch, std::vector<std::vector<cv::KeyPoint>> & inKeyPoints, const double & distanceThreshold);

//get aligned keypoints from DMatch information without specify distance
int formatKeypoints(std::vector<cv::KeyPoint> & outPoints, const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<std::vector<cv::DMatch>> & inMatch, std::vector<std::vector<cv::KeyPoint>> & inKeyPoints);

//uppdate keypoint matrces in newFrame according to the mached count
int matchForCamPose(const frame & keyframe, frame & newFrame, const std::vector<std::vector<cv::DMatch>> & inMatch);

//create new frame or key frame based on match
void vecKeypointToMatKeypoint(cv::Mat & keypointsMat, const std::vector<cv::KeyPoint> & keypointsVec);

//get aligned keypoints in cv::Mat format
cv::Mat vecKeypointToMatKeypoint(const std::vector<cv::KeyPoint> & keypointsVec);

double calKeypointDist(const cv::KeyPoint & pt1, const cv::KeyPoint & pt2);

//remove outliners from both keypoint list and matcher
int rmOutliner( std::vector<cv::KeyPoint> & inKeypointsNewframe, std::vector<cv::KeyPoint> & inKeypointsOldframe, std::vector<std::vector<cv::DMatch>> & inMatches, const int & threshold);

//get keypoints that matches
void getMatchedKeypoints(const std::vector<cv::KeyPoint> & query, const std::vector<cv::KeyPoint> & train, const std::vector<std::vector<cv::DMatch>> & matches, std::vector<cv::KeyPoint> & queryNew, std::vector<cv::KeyPoint> & trainNew);

#endif
