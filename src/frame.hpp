#include <assert.h>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/cuda.hpp>

#ifndef frame_h
#define frame_h

class frame
{
public:
  int keyframe; //index to keyframe current frame matched to
  //all keyPoints found in current frame
  std::vector<cv::KeyPoint> keypoint;
  cv::cuda::GpuMat descriptors;
  cv::Mat keypointToKeyframe; //the keypoint matrix aligned with keyframe
  cv::Mat keypointAsKeyframe; //only used when the frame is created as keyframe

  frame (const std::vector<cv::KeyPoint> & inKeypoints)
  {
    keypoint = inKeypoints;
    return;
  }

  frame (const std::vector<cv::KeyPoint> & inKeypoints, cv::cuda::GpuMat & inDesciptor)
  {
    keypoint = inKeypoints;
    descriptors = inDesciptor;
    return;
  }

};

#endif
