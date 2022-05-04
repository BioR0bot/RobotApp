#pragma once

#include "ui_camcalib.h"
#include "BasicWidget.h"
#include "memory"

class AppDataManager;
class SignalEmitter;


class CamCalibWidget : public BasicWidget
{
    Q_OBJECT

public:
    CamCalibWidget(std::shared_ptr<AppDataManager>& appDataManager, std::shared_ptr<SignalEmitter>& signalEmitter);
    void SetActive(bool val);
    void ProcessData();

private:
    void Start();
    void Prepearing();
    void TakingData();
    void Calibration();
    void CheckForFinished();
    void Finished();
    void StatusUpdate();


private:
    enum CamCalibStatus
    {
        UNDEFINED=0,
        PREPEARING,
        TAKING_DATA,
        CALIBRATION,
        CHECK_FOR_FINISHED,
        FINISHED
    };

    CamCalibStatus                  mCamCalibStatus;
    Ui::CamCalib                    mUi;
    std::shared_ptr<AppDataManager> mAppDataManager;
    std::shared_ptr<SignalEmitter>  mSignalEmitter;
};
