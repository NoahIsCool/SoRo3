#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QTimerEvent>

#include <Qt5GStreamer/QGst/Element>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGst/Message>

class VideoWindow
{
public:
    VideoWindow();

private:
    int _latency;
    QGst::PipelinePtr _pipeline;
    bool _playing;
    int _updateLatencyTimerId;
};

#endif // VIDEOWINDOW_H
