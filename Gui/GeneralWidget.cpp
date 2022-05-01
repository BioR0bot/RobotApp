#include "GeneralWidget.h"
#include "SignalEmitter.h"
#include "AppDataManager.h"


GeneralWidget::GeneralWidget(std::shared_ptr<AppDataManager> &appDataManager,
                             std::shared_ptr<SignalEmitter> &signalEmitter) :
    mAppDataManager(appDataManager),
    mSignalEmitter(signalEmitter)
{
    mUi.setupUi(this);

    connect(mUi.btnSettings, &QPushButton::released, mSignalEmitter.get(), &SignalEmitter::SetSettingsWidgetSignal);
    connect(mUi.btnCamCalib, &QPushButton::released, mSignalEmitter.get(), &SignalEmitter::SetmCamCalibWidgetSignal);
}

void GeneralWidget::SetActive(bool val)
{
    BasicWidget::SetActive(val);
    this->setVisible(val);
}

void GeneralWidget::ProcessData()
{

}
