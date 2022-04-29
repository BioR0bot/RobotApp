#-------------------------------------------------
#
# Project created by QtCreator 2022-04-26T23:44:24
#
#-------------------------------------------------

QT       += core gui\
            serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RobotApp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += object_parallel_to_source\
          c++17\
          console\

QMAKE_CXXFLAGS += -std=c++17\


INCLUDEPATH +=\
    "/usr/local/include/opencv4"\
    "$$PWD/ThirdParty/build/libdatachannel/include"\
    "$$PWD/ThirdParty/src/libdatachannel/deps/json/single_include"\
    "$$PWD/ThirdParty/build/qautostart/include"\

LIBS +=\
    -L"/usr/local/lib"\
    -lopencv_core\
    -lopencv_imgproc\
    -lopencv_videoio\
    -lopencv_highgui\
    -lopencv_imgcodecs\
    -L"$$PWD/ThirdParty/build/libdatachannel/lib"\
    -ldatachannel\
    -L"$$PWD/ThirdParty/build/qautostart/lib"\
    -lqautostart\



SOURCES += \
        main.cpp \
        mainwindow.cpp \
    AppDataManager.cpp \
    DatachannelThread.cpp \
    GeneralWidget.cpp \
    SettingsWidget.cpp \
    FramesCaptureThread.cpp \
    Constants.cpp

HEADERS += \
        mainwindow.h \
    AppDataManager.h \
    DatachannelThread.h \
    Utils.h \
    GeneralWidget.h \
    SignalEmitter.h \
    SettingsWidget.h \
    BasicWidget.h \
    FramesCaptureThread.h \
    Constants.h

FORMS += \
        mainwindow.ui \
    general.ui \
    settings.ui
