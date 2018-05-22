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

#include "gstreamerutil.h"
#include "logger.h"
#include "configreader.h"

const int FRONT_CAMERA_PORT = 5555;
const int BACK_CAMERA_PORT = 5556;
const int CLAW_CAMERA_PORT = 5557;

class VideoStreamer : public QObject
{
    Q_OBJECT
public:
    explicit VideoStreamer(QObject *parent = nullptr);
    VideoStreamer(QString configFile);

signals:

public slots:

private:
    const QString LOG_TAG = "VideoStreamer";
    GStreamerUtil::VideoProfile *profile;
    QGst::PipelinePtr frontPipeline;
    QGst::PipelinePtr backPipeline;
    QGst::PipelinePtr clawPipeline;

    void onBusMessage(const QGst::MessagePtr &message);
};

#endif // VIDEOSTREAMER_H
