#pragma once

#include "ui_settings.h"
#include "BasicWidget.h"
#include "memory"

class SignalEmitter;
class Autostart;

class SettingsWidget : public BasicWidget
{
    Q_OBJECT

public:
    SettingsWidget(std::shared_ptr<SignalEmitter>& signalEmitter);
    void SetActive(bool val);
    void ProcessData();

private:
    void SetAutostart(bool val);

private:
    Ui::Settings                   mUi;
    std::shared_ptr<SignalEmitter> mSignalEmitter;
    std::shared_ptr<Autostart>     mAutostart;
};
