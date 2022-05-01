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


private:
    Ui::CamCalib                    mUi;
    std::shared_ptr<AppDataManager> mAppDataManager;
    std::shared_ptr<SignalEmitter>  mSignalEmitter;
};
