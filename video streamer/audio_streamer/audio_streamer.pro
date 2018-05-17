QT += core network dbus

CONFIG += c++11 no_keywords
CONFIG += console
CONFIG -= app_bundle

TARGET = audio_streamer

BUILD_DIR = ../build/audio_streamer
DESTDIR = ../bin

TEMPLATE = app

HEADERS += audiostreamer.h

SOURCES += main.cpp \
    audiostreamer.cpp

# Include headers from other subprojects
INCLUDEPATH += $$PWD/..

#Link Qt5GStreamer
LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0 -lQt5GStreamerUtils-1.0

# Link against core
LIBS += -L../lib -lsoro_core
