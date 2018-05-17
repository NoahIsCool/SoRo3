QT += core network dbus

CONFIG += c++11 no_keywords
CONFIG += console
CONFIG -= app_bundle

TARGET = video_streamer

BUILD_DIR = ../build/video_streamer
DESTDIR = ../bin

TEMPLATE = app

HEADERS += \
    videostreamer.h

SOURCES += \
    main.cpp \
    videostreamer.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# Include headers from other subprojects
INCLUDEPATH += $$PWD/..

#Link Qt5GStreamer
LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0 -lQt5GStreamerUtils-1.0

# Link against core
LIBS += -L../lib -lsoro_core
