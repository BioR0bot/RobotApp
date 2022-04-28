#pragma once

#include <thread>
#include <atomic>
#include <memory>

class AppDataManager;

class FramesCaptureThread
{
public:
    FramesCaptureThread(std::shared_ptr<AppDataManager>& appDataManager);
    ~FramesCaptureThread();
    void Start();
    void Stop();

private:
    void ThreadFunction();

private:
    std::thread                     mThread;
    std::atomic_bool                mIsStopped;
    std::shared_ptr<AppDataManager> mAppDataManager;

};
