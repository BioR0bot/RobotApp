#pragma once

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <memory>

class AppDataManager;
class DatachannelThread;
class QTimer;

class SignalEmitter;
class GeneralWidget;
class SettingsWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void UpdateWidgets();
    void SetGeneralWidget();
    void SetSettingsWidget();

private:
    Ui::MainWindow mUi;

    std::shared_ptr<AppDataManager>    mAppDataManager;
    std::shared_ptr<DatachannelThread> mDatachannelThread;
    std::shared_ptr<QTimer>            mTimer;

    std::shared_ptr<SignalEmitter>     mSignalEmitter;
    std::shared_ptr<GeneralWidget>     mGeneralWidget;
    std::shared_ptr<SettingsWidget>    mSettingsWidget;
    QWidget*                           mLastAddedWidget;
};

