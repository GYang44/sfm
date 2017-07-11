#include "frameOperation.hpp"

int forkeypointsMat(const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<cv::KeyPoint> & inPointsNewframe, const std::vector<std::vector<cv::DMatch>> & inMatch, std::vector<cv::KeyPoint> & outKeyPoints, const double & distanceThreshold)
//create array of corresponding points for camera pose estimation
{
  outKeyPoints.clear();
  int matchedCount(0);
  //check distance
  for (uint inMatchCt(0); inMatchCt < inMatch.size(); inMatchCt++)
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

int forkeypointsMat(const std::vector<cv::KeyPoint> & inPointsKeyframe, const std::vector<cv::KeyPoint> & inPointsNewframe, const std::vector<std::vector<cv::DMatch>> & inMatch, std::vector<cv::KeyPoint> & outKeyPoints)
//create keypoints for camera pose estimation
{
  return forkeypointsMat(inPointsKeyframe, inPointsNewframe, inMatch, outKeyPoints, 20);
}

int matchForCamPose(const frame & keyframe, frame & newFrame, const std::vector<std::vector<cv::DMatch>> & inMatch)
{
  std::vector<cv::KeyPoint> tmpAlign;
  int matchCount = forkeypointsMat(keyframe.keypoint, newFrame.keypoint, inMatch, tmpAlign);
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

int matchForCamPoseDrawMatch(const frame & keyframe, frame & newFrame, std::vector<std::vector<cv::DMatch>> & inMatch)
{
  std::vector<cv::KeyPoint> tmpAlign;
  int matchCount = forkeypointsMat(keyframe.keypoint, newFrame.keypoint, inMatch, tmpAlign);
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

void vecKeypointToMatKeypoint(cv::Mat & keypointsMat, const std::vector<cv::KeyPoint> & keypointsVec)
{
  for (std::vector<cv::KeyPoint>::const_iterator keypointIt = keypointsVec.begin(); keypointIt != keypointsVec.end(); keypointIt++)
  {
    keypointsMat.push_back(cv::Mat(cv::Vec<double, 2>( keypointIt -> pt.x, keypointIt -> pt.y)).t());
  }
  keypointsMat = keypointsMat.t(); //return a 2 by n matrix
  return;
}

cv::Mat vecKeypointToMatKeypoint(const std::vector<cv::KeyPoint> & keypointsVec)
{
  cv::Mat keypointsMat;
  vecKeypointToMatKeypoint(keypointsMat, keypointsVec);
  return keypointsMat;
}

double calKeypointDist(const cv::KeyPoint & pt1, const cv::KeyPoint & pt2)
{
  return sqrt(pow((pt1.pt.x - pt2.pt.x),2) + pow((pt1.pt.y - pt2.pt.y),2));
}

//new frame is query frame old frame is train frame
int rmOutliner( std::vector<cv::KeyPoint> & inKeypointsNewframe, std::vector<cv::KeyPoint> & inKeypointsOldframe, std::vector<std::vector<cv::DMatch>> & inMatches, const int & threshold)
{
  if (inKeypointsOldframe.empty() | inKeypointsNewframe.empty()) return 0;
  int matchCount(0);

  std::vector<std::vector<cv::DMatch>> matchUpdated;

  for (uint queryIdx(0); queryIdx < inMatches.size(); queryIdx++)
  {
    int trainIdx = inMatches[queryIdx][0].trainIdx;//get matched point from new frame
    if ( calKeypointDist(inKeypointsOldframe[trainIdx], inKeypointsNewframe[queryIdx]) <= threshold)
    {

      cv::DMatch newMatchEntry(queryIdx, trainIdx, inMatches[queryIdx][0].distance);
      std::vector<cv::DMatch> matchTmp;
      matchTmp.push_back(newMatchEntry);

      matchUpdated.push_back(matchTmp);

      matchCount ++;
    }
  }

  inMatches = matchUpdated;

  return matchCount;
}

//query usually is occupided by keypoints from newFrame, train by oldFrame
void getMatchedKeypoints(const std::vector<cv::KeyPoint> & query, const std::vector<cv::KeyPoint> & train, const std::vector<std::vector<cv::DMatch>> & matches, std::vector<cv::KeyPoint> & queryNew, std::vector<cv::KeyPoint> & trainNew)
{
  trainNew.clear();
  queryNew.clear();

  int queryIdx, trainIdx;

  for(uint matchIdx(0); matchIdx < matches.size(); matchIdx++)
  {
    trainIdx = matches[matchIdx][0].trainIdx;
    queryIdx = matches[matchIdx][0].queryIdx;
    trainNew.push_back(train[trainIdx]);
    queryNew.push_back(query[queryIdx]);
  }

  return;
}
