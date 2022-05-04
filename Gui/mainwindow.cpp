#include "mainwindow.h"

#include <qtimer.h>
#include "AppDataManager.h"
#include "DatachannelThread.h"
#include "FramesCaptureThread.h"
#include "GeneralWidget.h"
#include "SettingsWidget.h"
#include "CamCalibWidget.h"
#include "SignalEmitter.h"

#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    mLastAddedWidget(NULL)
{
    mUi.setupUi(this);

    mAppDataManager = std::make_shared<AppDataManager>();
    mDatachannelThread = std::make_shared<DatachannelThread>(mAppDataManager);
    mFramesCaptureThread = std::make_shared<FramesCaptureThread>(mAppDataManager);
    mTimer = std::make_shared<QTimer>(this);
    mSignalEmitter = std::make_shared<SignalEmitter>();

    mGeneralWidget = std::make_shared<GeneralWidget>(mAppDataManager, mSignalEmitter);
    mSettingsWidget = std::make_shared<SettingsWidget>(mSignalEmitter);
    mCamCalibWidget = std::make_shared<CamCalibWidget>(mAppDataManager, mSignalEmitter);

    mDatachannelThread->Start();
    mFramesCaptureThread->Start();

    connect(mSignalEmitter.get(), &SignalEmitter::SetGeneralWidgetSignal, this, &MainWindow::SetGeneralWidget);
    connect(mSignalEmitter.get(), &SignalEmitter::SetSettingsWidgetSignal, this, &MainWindow::SetSettingsWidget);
    connect(mSignalEmitter.get(), &SignalEmitter::SetmCamCalibWidgetSignal, this, &MainWindow::SetCamCalibWidget);

    SetGeneralWidget();

    mTimer->singleShot(20, this, &MainWindow::UpdateWidgets);
}

MainWindow::~MainWindow()
{
}

void MainWindow::SetGeneralWidget()
{
    SetWidget(mGeneralWidget.get());
}

void MainWindow::SetSettingsWidget()
{
    SetWidget(mSettingsWidget.get());
}

void MainWindow::SetCamCalibWidget()
{
    SetWidget(mCamCalibWidget.get());
}

void MainWindow::SetWidget(QWidget *widget)
{
    if(mLastAddedWidget)
    {
        mUi.verticalLayout->removeWidget(mLastAddedWidget);
        ((BasicWidget*)mLastAddedWidget)->SetActive(false);
    }

    mUi.verticalLayout->insertWidget(0, widget);
    ((BasicWidget*)widget)->SetActive(true);
    mLastAddedWidget = widget;
}

void MainWindow::UpdateWidgets()
{
    mDatachannelThread->RestartIfNeed();

    if(mGeneralWidget->IsActive())
    {
        mGeneralWidget->ProcessData();
    }
    else if(mSettingsWidget->IsActive())
    {
        mSettingsWidget->ProcessData();
    }
    else if(mCamCalibWidget->IsActive())
    {
        mCamCalibWidget->ProcessData();
    }


    mTimer->singleShot(20, this, &MainWindow::UpdateWidgets);
}
