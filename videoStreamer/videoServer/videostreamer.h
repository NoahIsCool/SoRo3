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
#include <socket.h>
#include <soro_global.h>

#include "gstreamerutil.h"
#include "logger.h"
#include "configreader.h"

struct Camera{
    QString device;
    QHostAddress client;
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
    QTimer *heartbeatTimeout;
    bool connected;
    bool frontPipeEmpty = true;
    bool backPipeEmpty = true;
    bool clawPipeEmpty = true;
    QByteArray nope;

    void onBusMessage(const QGst::MessagePtr &message);
    void shutdownClientCameras(QHostAddress);
    void shutdownAllCameras();
    void startCamera(QString camera,QHostAddress clientAddress);
    void stopCamera(QString camera);
};

#endif // VIDEOSTREAMER_H
