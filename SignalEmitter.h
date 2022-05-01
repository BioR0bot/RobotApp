#pragma once

#include <qobject.h>

class SignalEmitter : public QObject
{
    Q_OBJECT

public:

signals:
    void SetGeneralWidgetSignal();
    void SetSettingsWidgetSignal();
    void SetmCamCalibWidgetSignal();
};
