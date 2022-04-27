#pragma once

#include "ui_general.h"
#include "BasicWidget.h"
#include "memory"

class SignalEmitter;
class AppDataManager;

class GeneralWidget : public BasicWidget
{
    Q_OBJECT

public:
    GeneralWidget(std::shared_ptr<AppDataManager>& appDataManager, std::shared_ptr<SignalEmitter>& signalEmitter);
    void SetActive(bool val);
    void ProcessData();

private:
    std::shared_ptr<AppDataManager> mAppDataManager;
    std::shared_ptr<SignalEmitter>  mSignalEmitter;
    Ui::General                     mUi;
};

