#include "DatachannelThread.h"
#include "AppDataManager.h"
#include "Utils.h"
#include <rtc/rtc.hpp>
#include <nlohmann/json.hpp>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

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
                size_t buffAmount = dcMapIt->second->bufferedAmount();
                size_t maxMessageSize = dcMapIt->second->maxMessageSize();

                if(buffAmount == 0)
                {
//                    cv::Mat frame;
//                    mAppDataManager->GetSurveillanceFrame(frame);

//                    cv::resize(frame, frame, cv::Size(320, 240));
//                    auto start = std::chrono::steady_clock::now();

//                    std::vector<int> imencodeParams;
//                    imencodeParams.push_back(cv::IMWRITE_JPEG_QUALITY);
//                    imencodeParams.push_back(75); // JPG quality
//                    std::vector<unsigned char> jpegBuf;
//                    cv::imencode(".jpg", frame, jpegBuf);

//                    auto end = std::chrono::steady_clock::now();
//                    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;

//                    if(jpegBuf.size() < maxMessageSize)
//                    {
//                        dcMapIt->second->send((std::byte*)(jpegBuf.data()), jpegBuf.size());
//                    }

                    dcMapIt->second->send(std::to_string(testVal));
                }

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

