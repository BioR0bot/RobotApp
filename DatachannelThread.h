#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <unordered_map>

class AppDataManager;

namespace rtc
{
    class PeerConnection;
    struct Configuration;
    class WebSocket;
    class DataChannel;
} // namespace rtc

class DatachannelThread
{
public:
    DatachannelThread(std::shared_ptr<AppDataManager>& appDataManager);
    ~DatachannelThread();
    void Start();
    void Stop();
    void RestartIfNeed();

private:
    enum ConnectStatus
    {
        UNDEFINED=0,
        CONNECTED,
        ERROR,
        CLOSED
    };

private:
    void ThreadFunction();
    void WaitForWebSockConnection(int millisecWait, ConnectStatus& connectStatus);
    std::shared_ptr<rtc::PeerConnection> CreatePeerConnection(
            const rtc::Configuration& config, std::weak_ptr<rtc::WebSocket> wws, std::string id,
            std::shared_ptr<AppDataManager> appDataManager,
            std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>>& peerConnectionMap,
            std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>>& dataChannelMap);

private:
    std::thread                     mThread;
    std::atomic_bool                mIsStopped;
    std::shared_ptr<AppDataManager> mAppDataManager;
    std::atomic_bool                mNeedToRestart;
};
