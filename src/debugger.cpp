#include "debugger.hpp"


template<typename Tp_>
void printMat(const cv::Mat & mat)
{
    std::cout << std::endl;
    for (int row(0); row < mat.size().height; row++)
    {
        for (int col(0); col < mat.size().width; col++)
        {
            std::cout << mat.at<Tp_>(row,col) << ' ';
        }
        std::cout << std::endl;
    }
    return;
}

void trackMatches(cv::Mat & frame, const std::vector<cv::KeyPoint> & newKeyPoints, const std::vector<cv::KeyPoint> & oldKeyPoints)
{
  for(uint i(0); i < newKeyPoints.size(); i++)
  {
    cv::line(frame, newKeyPoints[i].pt, oldKeyPoints[i].pt, cv::Scalar(94,165,255,255),5);
  }
}
