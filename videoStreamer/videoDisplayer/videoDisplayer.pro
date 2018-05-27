QT -= gui
QT += network core dbus widgets multimediawidgets

CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += link pkgconfig

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    configreader.cpp \
    logger.cpp \
    mpvlauncher.cpp \
    player.cpp \
    gstreamerutil.cpp

HEADERS += \
    configreader.h \
    logger.h \
    mpvlauncher.h \
    player.h \
    gstreamerutil.h \
    multisink.h

INCLUDEPATH += ../Networking/

PKGCONFIG += QtGStreamer-1.0 QtGLib-2.0 QtGstreamerUtils-1.0 QtGStreamerUi-1.0

LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0 -lQt5GStreamerUtils-1.0 -lQt5GStreamerUi-1.0 -L../build-Networking-Desktop_Qt_5_9_1_GCC_64bit-Debug/ -lNetworking
