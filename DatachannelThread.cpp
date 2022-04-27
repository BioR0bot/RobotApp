#include "DatachannelThread.h"
#include "AppDataManager.h"
#include "Utils.h"
#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>

DatachannelThread::DatachannelThread(std::shared_ptr<AppDataManager>& appDataManager) :
    mIsStopped(true),
    mAppDataManager(appDataManager),
    mNeedToRestart(false)
{
}

DatachannelThread::~DatachannelThread()
{
    Stop();
}

void DatachannelThread::Start()
{
    Stop();
    mIsStopped.store(false);
    mThread = std::thread(&DatachannelThread::ThreadFunction, this);
}

void DatachannelThread::Stop()
{
    mIsStopped.store(true);
    if(mThread.joinable())
    {
        mThread.join();
    }
}

void DatachannelThread::RestartIfNeed()
{
    if(mNeedToRestart.load())
    {
        mNeedToRestart.store(false);
        Start();
    }
}

void DatachannelThread::ThreadFunction()
{
    // Remove later
    int testVal = 0;



    std::unordered_map<std::string, std::shared_ptr<rtc::PeerConnection>> peerConnectionMap;
    std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel>> dataChannelMap;

    rtc::Configuration peerConnectConfig;
    const rtc::IceServer turnServer("handsome.edgeconsult.me", 3478, "guest", "somepassword");
    peerConnectConfig.iceServers.emplace_back(turnServer);

    std::function<void(rtc::LogLevel level, std::string message)> logCallback =
            [](rtc::LogLevel level, std::string message) { std::cout << message << std::endl; };

    rtc::InitLogger(rtc::LogLevel::None, logCallback);

    rtc::WebSocket::Configuration websockConfig;
    websockConfig.disableTlsVerification = true;

    auto ws = std::make_shared<rtc::WebSocket>(websockConfig);

    ConnectStatus connectStatus = ConnectStatus::UNDEFINED;

    ws->onOpen([&]()
    {
        connectStatus = ConnectStatus::CONNECTED;
    });

    ws->onError([&](std::string s)
    {
        connectStatus = ConnectStatus::ERROR;
    });

    ws->onClosed([&]()
    {
        connectStatus = ConnectStatus::CLOSED;
    });

    ws->onMessage([&](std::variant<rtc::binary, std::string> data)
    {
        if(!std::holds_alternative<std::string>(data))
        {
            return;
        }

        nlohmann::json message = nlohmann::json::parse(std::get<std::string>(data));

        auto it = message.find("id");
        if (it == message.end())
        {
            return;
        }
        std::string id = it->get<std::string>();

        it = message.find("type");
        if(it == message.end())
        {
            return;
        }
        std::string type = it->get<std::string>();

        std::shared_ptr<rtc::PeerConnection> pc;
        if(auto jt = peerConnectionMap.find(id); jt != peerConnectionMap.end())
        {
            pc = jt->second;
        }
        else if(type == "offer")
        {
            pc = CreatePeerConnection(peerConnectConfig, ws, id, mAppDataManager, peerConnectionMap, dataChannelMap);
        }
        else
        {
            return;
        }

        if(type == "offer" || type == "answer")
        {
            auto sdp = message["description"].get<std::string>();
            pc->setRemoteDescription(rtc::Description(sdp, type));
        }
        else if(type == "candidate")
        {
            auto sdp = message["candidate"].get<std::string>();
            auto mid = message["mid"].get<std::string>();
            pc->addRemoteCandidate(rtc::Candidate(sdp, mid));
        }
    });


    std::string localId = mAppDataManager->mLocalId;
    const std::string url = "wss://handsome.edgeconsult.me/" + localId;

    try
    {
        ws->open(url);
    }
    catch(...)
    {
       std::this_thread::sleep_for(std::chrono::milliseconds(500));
       mNeedToRestart.store(true);
       return;
    }

    WaitForWebSockConnection(5000, connectStatus);

    int millisecPast = 0;
    const int millisecStep = 10;

    while(!mIsStopped.load())
    {
        if(connectStatus != ConnectStatus::CONNECTED)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            mNeedToRestart.store(true);
            return;
        }

        auto dcMapIt = dataChannelMap.find(mAppDataManager->mExternalId);
        if(dcMapIt != dataChannelMap.end() && dcMapIt->second->isOpen())
        {
            millisecPast = 0;

            try
            {
                dcMapIt->second->send(std::to_string(testVal));
            }
            catch(...)
            {
                mNeedToRestart.store(true);
                return;
            }

            testVal++;
        }
        else
        {
            millisecPast += millisecStep;
            if(millisecPast > 4000)
            {
                mNeedToRestart.store(true);
                return;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(millisecStep));
    }
}

void DatachannelThread::WaitForWebSockConnection(int millisecWait, ConnectStatus& connectStatus)
{
    int millisecPast = 0;
    int millisecStep = 10;

    while(!mIsStopped.load() && connectStatus == ConnectStatus::UNDEFINED && millisecPast < millisecWait)
    {
        millisecPast += millisecStep;
        std::this_thread::sleep_for(std::chrono::milliseconds(millisecStep));
    }
}

std::shared_ptr<rtc::PeerConnection> DatachannelThread::CreatePeerConnection(
        const rtc::Configuration &config, std::weak_ptr<rtc::WebSocket> wws, std::string id,
        std::shared_ptr<AppDataManager> appDataManager, std::unordered_map<std::string,
        std::shared_ptr<rtc::PeerConnection> > &peerConnectionMap,
        std::unordered_map<std::string, std::shared_ptr<rtc::DataChannel> > &dataChannelMap)
{
    auto pc = std::make_shared<rtc::PeerConnection>(config);

    pc->onStateChange([](rtc::PeerConnection::State state)
    {
    });

    pc->onGatheringStateChange([](rtc::PeerConnection::GatheringState state)
    {
    });

    pc->onLocalDescription([wws, id](rtc::Description description)
    {
        nlohmann::json message =
        {
            {"id", id}, {"type", description.typeString()}, {"description", std::string(description)}
        };

        if(auto ws = wws.lock())
        {
            ws->send(message.dump());
        }
    });

    pc->onLocalCandidate([wws, id](rtc::Candidate candidate)
    {
        nlohmann::json message =
        {
            {"id", id},
            {"type", "candidate"},
            {"candidate", std::string(candidate)},
            {"mid", candidate.mid()}
        };

        if(auto ws = wws.lock())
        {
            ws->send(message.dump());
        }
    });

    pc->onDataChannel([id, appDataManager, &dataChannelMap](std::shared_ptr<rtc::DataChannel> dc)
    {
        dc->onOpen([wdc = make_weak_ptr(dc), appDataManager]()
        {
            if(auto dc = wdc.lock())
            {
                dc->send("Hello from " + appDataManager->mLocalId);
            }
        });

        dc->onClosed([id]()
        {
        });

        dc->onMessage([id](std::variant<rtc::binary, std::string> data)
        {
            if(std::holds_alternative<std::string>(data))
            {
            }
            else
            {
            }
        });

        dataChannelMap.emplace(id, dc);
    });

    peerConnectionMap.emplace(id, pc);
    return pc;
}

