#ifndef VIDEOWINDOW_H
#define VIDEOWINDOW_H

#include <QObject>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include <QTimerEvent>

#include <Qt5GStreamer/QGst/Element>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Bin>
#include <Qt5GStreamer/QGst/Message>

class VideoWindow
{
public:
    VideoWindow(QQmlEngine *engine);

private:
    int _latency;
    QGst::PipelinePtr _pipeline;
    bool _playing;
    QQuickWindow *_window = 0;
    int _updateLatencyTimerId;
};

#endif // VIDEOWINDOW_H
