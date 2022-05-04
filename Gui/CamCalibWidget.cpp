#include "CamCalibWidget.h"
#include "SignalEmitter.h"
#include "AppDataManager.h"
#include "Utils.h"
#include "Constants.h"

CamCalibWidget::CamCalibWidget(std::shared_ptr<AppDataManager> &appDataManager,
                               std::shared_ptr<SignalEmitter> &signalEmitter):
    mAppDataManager(appDataManager),
    mSignalEmitter(signalEmitter),
    mCamCalibStatus(CamCalibStatus::UNDEFINED)
{
    mUi.setupUi(this);

    connect(mUi.btnReturn, &QPushButton::released, mSignalEmitter.get(), &SignalEmitter::SetGeneralWidgetSignal);
    connect(mUi.btnStart, &QPushButton::released, this, &CamCalibWidget::Start);
}

void CamCalibWidget::SetActive(bool val)
{
    BasicWidget::SetActive(val);
    this->setVisible(val);
}

void CamCalibWidget::ProcessData()
{
    cv::Mat leftFrame;
    cv::Mat rightFrame;
    mAppDataManager->GetStereoFrames(leftFrame, rightFrame);

    mUi.lblFrameLeft->setPixmap(CvMatToQPixmap(leftFrame));
    mUi.lblFrameRight->setPixmap(CvMatToQPixmap(rightFrame));

    StatusUpdate();
}

void CamCalibWidget::Start()
{
    mUi.btnStart->setEnabled(false);
    mUi.btnStart->setStyleSheet(Constants::mDisabledButtonStyleSheet);
    mUi.btnReturn->setEnabled(false);
    mUi.btnReturn->setStyleSheet(Constants::mDisabledButtonStyleSheet);
    mCamCalibStatus = CamCalibStatus::PREPEARING;
}

void CamCalibWidget::Prepearing()
{




    mCamCalibStatus = CamCalibStatus::TAKING_DATA;
}

void CamCalibWidget::TakingData()
{
    mCamCalibStatus = CamCalibStatus::CALIBRATION;
}

void CamCalibWidget::Calibration()
{
    mCamCalibStatus = CamCalibStatus::CHECK_FOR_FINISHED;
}

void CamCalibWidget::CheckForFinished()
{
    bool isFinished = true;

    if(isFinished)
    {
        mCamCalibStatus = CamCalibStatus::FINISHED;
    }
}

void CamCalibWidget::Finished()
{
    mUi.btnStart->setEnabled(true);
    mUi.btnStart->setStyleSheet(Constants::mEnabledButtonStyleSheet);
    mUi.btnReturn->setEnabled(true);
    mUi.btnReturn->setStyleSheet(Constants::mEnabledButtonStyleSheet);
    mUi.lblStatus->setText("Calibration finished");
    mCamCalibStatus = CamCalibStatus::UNDEFINED;
}

void CamCalibWidget::StatusUpdate()
{
    switch(mCamCalibStatus)
    {
    case CamCalibStatus::UNDEFINED:
        break;
    case CamCalibStatus::PREPEARING:
        Prepearing();
        break;
    case CamCalibStatus::TAKING_DATA:
        TakingData();
        break;
    case CamCalibStatus::CALIBRATION:
        Calibration();
        break;
    case CamCalibStatus::CHECK_FOR_FINISHED:
        CheckForFinished();
        break;
    case CamCalibStatus::FINISHED:
        Finished();
        break;
    default:
        break;
    }

}
