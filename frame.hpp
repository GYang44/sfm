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
    frame * keyframe = NULL; //pointer to keyframe current frame matched to
    std::vector<cv::KeyPoint> keyPoint;
    frame(std::vector<cv::KeyPoint> & inKeyPoints)
    {
        keyPoint = inKeyPoints;
        return;
    }
};

class keyframe : public frame
{
public:
    cv::cuda::GpuMat descriptors;
    keyframe(frame base):frame(base)
    {
        //keyframe = -1;
        return;
    }
    keyframe(const frame & baseFrame, cv::cuda::GpuMat & inMat):frame(baseFrame)
    {
        descriptors = inMat;
        //keyframeIdx = -1;
        return;
    }
};

#endif
