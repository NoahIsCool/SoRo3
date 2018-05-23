#ifndef MPVLAUNCHER_H
#define MPVLAUNCHER_H
#include<QTextStream>
#include<iostream>
#include <QTimer>
#include <QHostAddress>
#include <QObject>
#include <thread>

#include "socket.h"
#include "configreader.h"
#include "logger.h"

const int FRONT_CAMERA_PORT     = 5555;
const int BACK_CAMERA_PORT      = 5556;
const int CLAW_CAMERA_PORT      = 5557;
const int CONTROL_PORT          = 6969;
const int HEARTBEAT_PORT        = 6970;

const int CONTROL_CLIENT_PORT   = 6971;
const int HEARTBEAT_CLIENT_PORT = 6972;

class MPVLauncher : public QObject
{
    Q_OBJECT
public:
    explicit MPVLauncher(QObject *parent = nullptr);
    void start();

signals:

public slots:
    void beatHeart();
    void checkHeartBeat(DataPacket packet);

private:
    const QString LOG_TAG = "Video Displayer";
    QHostAddress *rover;
    socket *control;
    socket *heartbeat;
    QTimer *timer;
    QByteArray *controlPacket;
    std::thread *inputThread;

    void processInput();
};

#endif // MPVLAUNCHER_H
