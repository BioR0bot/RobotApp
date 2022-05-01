#include "FramesCaptureThread.h"
#include "Constants.h"
#include "AppDataManager.h"

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

FramesCaptureThread::FramesCaptureThread(std::shared_ptr<AppDataManager>& appDataManager) :
    mIsStopped(true),
    mAppDataManager(appDataManager)
{
}

FramesCaptureThread::~FramesCaptureThread()
{
    Stop();
}

void FramesCaptureThread::Start()
{
    Stop();
    mIsStopped.store(false);
    mThread = std::thread(&FramesCaptureThread::ThreadFunction, this);
}

void FramesCaptureThread::Stop()
{
    mIsStopped.store(true);
    if(mThread.joinable())
    {
        mThread.join();
    }
}

void FramesCaptureThread::ThreadFunction()
{
    cv::VideoCapture leftCam;
    bool leftCamOpened = leftCam.open(1, cv::CAP_V4L2);
    leftCam.set(cv::CAP_PROP_FRAME_WIDTH, Constants::mStereoFrameSize.width);
    leftCam.set(cv::CAP_PROP_FRAME_HEIGHT, Constants::mStereoFrameSize.height);

    cv::VideoCapture rightCam;
    bool rightCamOpened = rightCam.open(0, cv::CAP_V4L2);
    rightCam.set(cv::CAP_PROP_FRAME_WIDTH, Constants::mStereoFrameSize.width);
    rightCam.set(cv::CAP_PROP_FRAME_HEIGHT, Constants::mStereoFrameSize.height);

    if(!(leftCamOpened && rightCamOpened))
    {
        Stop();
    }

    while(!mIsStopped.load())
    {
        bool leftGrabSuccess = leftCam.grab();
        bool rightGrabSuccess = rightCam.grab();

        if(leftGrabSuccess && rightGrabSuccess)
        {
            cv::Mat leftFrame;
            cv::Mat rightFrame;

            leftCam.retrieve(leftFrame);
            rightCam.retrieve(rightFrame);

            if(!(leftFrame.empty() || rightFrame.empty()))
            {
                cv::cvtColor(leftFrame, leftFrame, cv::COLOR_BGR2GRAY);
                cv::cvtColor(rightFrame, rightFrame, cv::COLOR_BGR2GRAY);

                mAppDataManager->SetStereoFrames(leftFrame, rightFrame);
            }

        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}
