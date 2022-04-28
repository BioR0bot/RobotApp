#pragma once

#include <string>
#include <mutex>
#include <opencv2/core.hpp>

class AppDataManager
{
public:
    AppDataManager();
    void SetStereoFrames(const cv::Mat& left, const cv::Mat& right);
    void GetStereoFrames(cv::Mat& left, cv::Mat& right);

public:
    const std::string mLocalId;
    const std::string mExternalId;

private:
    cv::Mat mStereoFrameLeft;
    cv::Mat mStereoFrameRight;
    std::mutex mStereoMutex;

};
