#include <assert.h>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/core/cuda.hpp>

#ifndef fm_h
#define fm_h

class frame
{
public:
    int keyframe; //index to keyframe current frame matched to
    std::vector<cv::KeyPoint> keypoint;
    cv::cuda::GpuMat descriptors;
    cv::Mat keypointToKeyframe; //the keypoint matrix aligned with keyframe
    cv::Mat keypointAsKeyframe; //only used when the frame is created as keyframe

    frame (const std::vector<cv::KeyPoint> & inKeypoints)
    {
        keypoint = inKeypoints;
        return;
    }

    frame (const std::vector<cv::KeyPoint> & inKeypoints, cv::cuda::GpuMat & inMat)
    {
        keypoint = inKeypoints;
        descriptors = inMat;
        return;
    }

};


#endif
