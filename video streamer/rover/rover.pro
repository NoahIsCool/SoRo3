QT += core network

CONFIG += c++11 no_keywords
CONFIG += console
CONFIG -= app_bundle
TARGET = rover
TEMPLATE = app

BUILD_DIR = ../build/rover
DESTDIR = ../bin
OBJECTS_DIR = $$BUILD_DIR
MOC_DIR = $$BUILD_DIR
RCC_DIR = $$BUILD_DIR
UI_DIR = $$BUILD_DIR
PRECOMPILED_DIR = $$BUILD_DIR

HEADERS += \
    maincontroller.h \
    mediaserver.h \
    videoserver.h \
    audioserver.h \
    gpsserver.h \
    usbcameraenumerator.h

SOURCES += \
    main.cpp \
    maincontroller.cpp \
    mediaserver.cpp \
    videoserver.cpp \
    audioserver.cpp \
    gpsserver.cpp \
    usbcameraenumerator.cpp

#Link Qt5GStreamer
LIBS += -lQt5GStreamer-1.0 -lQt5GLib-2.0 -lQt5GStreamerUtils-1.0

# Include headers from other subprojects
INCLUDEPATH += $$PWD/..

LIBS += -L../lib -lsoro_core
