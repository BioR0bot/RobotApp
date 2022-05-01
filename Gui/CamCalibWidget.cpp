#include "CamCalibWidget.h"
#include "SignalEmitter.h"
#include "AppDataManager.h"
#include "Utils.h"

CamCalibWidget::CamCalibWidget(std::shared_ptr<AppDataManager> &appDataManager,
                               std::shared_ptr<SignalEmitter> &signalEmitter):
    mAppDataManager(appDataManager),
    mSignalEmitter(signalEmitter)
{
    mUi.setupUi(this);

    connect(mUi.btnReturn, &QPushButton::released, mSignalEmitter.get(), &SignalEmitter::SetGeneralWidgetSignal);
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

}
