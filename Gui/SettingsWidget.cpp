#include "SettingsWidget.h"
#include "SignalEmitter.h"
#include <QAutostart>

SettingsWidget::SettingsWidget(std::shared_ptr<SignalEmitter> &signalEmitter) :
    mSignalEmitter(signalEmitter)
{
    mAutostart = std::make_shared<Autostart>();

    mUi.setupUi(this);
    mUi.chbxEnableAutostart->setChecked(mAutostart->isAutostart());

    connect(mUi.btnReturn, &QPushButton::released, mSignalEmitter.get(), &SignalEmitter::SetGeneralWidgetSignal);
    connect(mUi.chbxEnableAutostart, &QCheckBox::toggled, this, &SettingsWidget::SetAutostart);
}

void SettingsWidget::SetActive(bool val)
{
    BasicWidget::SetActive(val);
    this->setVisible(val);
}

void SettingsWidget::ProcessData()
{

}

void SettingsWidget::SetAutostart(bool val)
{
    mAutostart->setAutostart(val);
}
