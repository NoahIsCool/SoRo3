#ifndef MPVLAUNCHER_H
#define MPVLAUNCHER_H
#include <QTimer>
#include <QHostAddress>
#include <QObject>
#include <thread>
#include <iostream>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Parse>

#include "socket.h"
#include "soro_global.h"
#include "configreader.h"
#include "logger.h"
#include "player.h"
#include "multisink.h"

using namespace soro;

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
    void onMessage(DataPacket packet);
    void attemptConnection();

private:
    const QString LOG_TAG = "Video Displayer";
    QHostAddress *rover;
    QHostAddress *forwardedTo;
    //socket *forwardedTo;
    socket *control;
    socket *heartbeat;
    QTimer *heartbeatTimer;
    QTimer *connectTimer;
    QByteArray *initMessage;
    std::thread *inputThread;
    QGst::PipelinePtr frontPipeline;
    QGst::PipelinePtr displayPipeline;
    QGst::PipelinePtr backPipeline;
    QGst::PipelinePtr clawPipeline;
    MultiSink *frontSink;
    bool connected;
    bool isMaster;
    QHostAddress *master;

    void processInput();
};

#endif // MPVLAUNCHER_H
