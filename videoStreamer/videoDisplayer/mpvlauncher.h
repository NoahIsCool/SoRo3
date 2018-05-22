#ifndef MPVLAUNCHER_H
#define MPVLAUNCHER_H
#include <iostream>
#include <QTimer>
#include <QHostAddress>

#include "socket.h"
#include "configreader.h"
#include "logger.h"

const int FRONT_CAMERA_PORT     = 5555;
const int BACK_CAMERA_PORT      = 5556;
const int CLAW_CAMERA_PORT      = 5557;
const int CONTROL_PORT          = 6969;
const int HEARTBEAT_PORT        = 6970;

using namespace Logger;

class MPVLauncher : public QObject
{
    Q_OBJECT
public:
    MPVLauncher(QObject *parent = nullptr);

public slots:
    void beatHeart();

private:
    const QString LOG_TAG = "Video Displayer";
    QHostAddress *rover;
    socket *control;
    socket *heartbeat;
    QTimer *timer;
};

#endif // MPVLAUNCHER_H
