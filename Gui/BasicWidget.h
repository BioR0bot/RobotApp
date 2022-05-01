#pragma once

#include <qwidget.h>

class BasicWidget : public QWidget
{
    Q_OBJECT

public:
    BasicWidget(){mIsActive = false;}
    virtual void SetActive(bool val){ mIsActive = val; }
    bool IsActive() {return mIsActive;}

private:
    bool mIsActive;

};
