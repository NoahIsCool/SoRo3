QT -= gui
QT += network core dbus widgets qml quick quickcontrols2 webengine

CONFIG += c++11 console
CONFIG -= app_bundle
CONFIG += pkgconfig

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
    videowindow.cpp \
    socket.cpp \
    configreader.cpp \
    logger.cpp \
    mpvlauncher.cpp

HEADERS += \
    videowindow.h \
    socket.h \
    configreader.h \
    logger.h \
    mpvlauncher.h

PKGCONFIG += QtGStreamer-1.0 Qt5GLib-2.0

LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0 -lQt5GStreamerUtils-1.0

DISTFILES += \
    VideoWindow.qml
