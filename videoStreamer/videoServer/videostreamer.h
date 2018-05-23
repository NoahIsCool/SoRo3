#ifndef VIDEOSTREAMER_H
#define VIDEOSTREAMER_H

#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGlib/Error>
#include <Qt5GStreamer/QGlib/Connect>
#include <Qt5GStreamer/QGst/Init>
#include <Qt5GStreamer/QGst/Bus>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Parse>
#include <Qt5GStreamer/QGst/Message>
#include <Qt5GStreamer/QGst/Utils/ApplicationSink>
#include <Qt5GStreamer/QGst/Utils/ApplicationSource>
#include <Qt5GStreamer/QGlib/Init>
#include <QTimer>

#include "gstreamerutil.h"
#include "logger.h"
#include "configreader.h"
#include "socket.h"

const int FRONT_CAMERA_PORT     = 5555;
const int BACK_CAMERA_PORT      = 5556;
const int CLAW_CAMERA_PORT      = 5557;
const int CONTROL_PORT          = 6969;
const int HEARTBEAT_PORT        = 6970;

const int CONTROL_CLIENT_PORT   = 6971;
const int HEARTBEAT_CLIENT_PORT = 6972;

enum CAMERA : int{
    FRONT = 0,
    BACK,
    CLAW,
};

class VideoStreamer : public QObject
{
    Q_OBJECT
public:
    explicit VideoStreamer(QObject *parent = nullptr);
    VideoStreamer(QString configFile);

signals:

public slots:
    void onMessage(DataPacket packet);
    void onHeartbeat(DataPacket packet);
    void onTimeout();

private:
    const QString LOG_TAG = "VideoStreamer";
    GStreamerUtil::VideoProfile *profile;
    QGst::PipelinePtr frontPipeline;
    QGst::PipelinePtr backPipeline;
    QGst::PipelinePtr clawPipeline;
    QString frontDevice = "NOT_FOUND";
    QString backDevice = "NOT_FOUND";
    QString clawDevice = "NOT_FOUND";

    socket *heartbeat;
    QHostAddress heartbeatAddress;
    socket *control;
    QTimer *timer;
    bool connected;

    void onBusMessage(const QGst::MessagePtr &message);
    void shutdownAllCameras();
    void startCamera(CAMERA camera,QHostAddress clientAddress);
};

#endif // VIDEOSTREAMER_H
