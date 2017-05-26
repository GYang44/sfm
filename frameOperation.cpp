#include "frameOperation.hpp"
#include "debugger.hpp"

int formatKeypoints(const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<cv::KeyPoint> & inPointsNewframe, const std::vector<std::vector<cv::DMatch>> & inMatch, std::vector<cv::KeyPoint> & outKeyPoints, const double & distanceThreshold)
//create array of corresponding points for camera pose estimation
{
  outKeyPoints.clear();
  int matchedCount(0);
  //check distance
  for (int inMatchCt(0); inMatchCt < inMatch.size(); inMatchCt++)
  {
    int newFrameKeypointNum = inMatch[inMatchCt][0].queryIdx;//get matched point from new frame
    if ( calKeypointDist(inPointsKeyframe[inMatchCt], inPointsNewframe[newFrameKeypointNum]) <= distanceThreshold)
    {
      outKeyPoints.push_back(inPointsNewframe[newFrameKeypointNum]); //store matched points from current frame
      matchedCount ++;
    }
    else
    {
      outKeyPoints.push_back(cv::KeyPoint(-1,-1, 0)); //store matched points from current frame
    }
  }
  return matchedCount;

}

int formatKeypoints(const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<cv::KeyPoint> & inPointsNewframe, const std::vector<std::vector<cv::DMatch>> & inMatch, std::vector<cv::KeyPoint> & outKeyPoints)
//create keypoints for camera pose estimation
{
  return formatKeypoints(inPointsKeyframe, inPointsNewframe, inMatch, outKeyPoints, 20);
}

int matchForCamPose(const frame & keyframe, frame & newFrame, const std::vector<std::vector<cv::DMatch>> & inMatch)
{
  std::vector<cv::KeyPoint> tmpAlign;
  int matchCount = formatKeypoints(keyframe.keypoint, newFrame.keypoint, inMatch, tmpAlign);
  //todo convert tmp algin based matched count
  if (matchCount > 7)
  {
    // store match to keyframe
    newFrame.keypointToKeyframe = vecKeypointToMatKeypoint(tmpAlign);

    if (matchCount <= 10)
    {
      //create newKeyframe
      newFrame.keypointAsKeyframe = vecKeypointToMatKeypoint(newFrame.keypoint);
    }
  }
  return matchCount;
}

int matchForCamPoseDrawMatch(const frame & keyframe, frame & newFrame, const std::vector<std::vector<cv::DMatch>> & inMatch)
{
  std::vector<cv::KeyPoint> tmpAlign;
  int matchCount = formatKeypoints(keyframe.keypoint, newFrame.keypoint, inMatch, tmpAlign);
  //todo convert tmp algin based matched count
  if (matchCount > 7)
  {
    // store match to keyframe
    newFrame.keypointToKeyframe = vecKeypointToMatKeypoint(tmpAlign);

    if (matchCount <= 10)
    {
      //create newKeyframe
      newFrame.keypointAsKeyframe = vecKeypointToMatKeypoint(newFrame.keypoint);
    }
  }
  return matchCount;
}

void vecKeypointToMatKeypoint(cv::Mat & matKeypoints, const std::vector<cv::KeyPoint> & vecKeypoints)
{
  for (std::vector<cv::KeyPoint>::const_iterator keypointIt = vecKeypoints.begin(); keypointIt != vecKeypoints.end(); keypointIt++)
  {
    matKeypoints.push_back(cv::Mat(cv::Vec<double, 2>( keypointIt -> pt.x, keypointIt -> pt.y)).t());
  }
  matKeypoints = matKeypoints.t(); //return a 2 by n matrix
  return;
}

cv::Mat vecKeypointToMatKeypoint(const std::vector<cv::KeyPoint> & vecKeypoints)
{
  cv::Mat matKeypoints;
  vecKeypointToMatKeypoint(matKeypoints, vecKeypoints);
  return matKeypoints;
}

double calKeypointDist(const cv::KeyPoint & pt1, const cv::KeyPoint & pt2)
{
  return sqrt(pow((pt1.pt.x - pt2.pt.x),2) + pow((pt1.pt.y - pt2.pt.y),2));
}
