#include <iostream>
#include <opencv2/core.hpp>

#ifndef DEBUGGER_H
#define DEBUGGER_H

template<typename Tp>
void printMat(const cv::Mat & mat)
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
  return;
}

#endif