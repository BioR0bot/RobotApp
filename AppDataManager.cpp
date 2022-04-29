#include "AppDataManager.h"
#include "Constants.h"

AppDataManager::AppDataManager() :
        mLocalId("Robot"),
        mExternalId("Mobil")
{
    mStereoFrameLeft = cv::Mat(Constants::mStereoFrameSize, CV_8UC1, cv::Scalar(0, 0, 0));
    mStereoFrameRight = cv::Mat(Constants::mStereoFrameSize, CV_8UC1, cv::Scalar(0, 0, 0));;
}

void AppDataManager::SetStereoFrames(const cv::Mat &left, const cv::Mat &right)
{
    std::lock_guard<std::mutex> lock(mStereoMutex);
    left.copyTo(mStereoFrameLeft);
    right.copyTo(mStereoFrameRight);
}

void AppDataManager::GetStereoFrames(cv::Mat &left, cv::Mat &right)
{
    std::lock_guard<std::mutex> lock(mStereoMutex);
    mStereoFrameLeft.copyTo(left);
    mStereoFrameRight.copyTo(right);
}

void AppDataManager::GetSurveillanceFrame(cv::Mat &frame)
{
     std::lock_guard<std::mutex> lock(mStereoMutex);
     mStereoFrameLeft.copyTo(frame);
}
