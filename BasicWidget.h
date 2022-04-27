#pragma once

#include <qwidget.h>

class BasicWidget : public QWidget
{
    Q_OBJECT

public:
    BasicWidget(){mIsActive = false;}
    virtual void SetActive(bool isActive){ mIsActive = isActive; }
    bool IsActive() {return mIsActive;}

private:
    bool mIsActive;

};
