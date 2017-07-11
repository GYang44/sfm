#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#ifndef debugger_h
#define debugger_h

template<typename Tp_>
void printMat(const cv::Mat & mat);

void trackMatches(cv::Mat & frame, const std::vector<cv::KeyPoint> & newKeyPoints, const std::vector<cv::KeyPoint> & oldKeyPoints);

#endif
