#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

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
#include <Qt5GStreamer/QGst/Ui/VideoWidget>
#include <QWidget>

class VideoPlayer : public QWidget
{
    Q_OBJECT
public:
    VideoPlayer(QWidget *parent = 0);

Q_SIGNALS:
    void stateChanged();

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void onPipelineChange(const QGst::StateChangedMessagePtr & scm);

    QWidget *video;
    //QGst::Ui::VideoWidget *video;
    QGst::PipelinePtr pipeline;
    QTimer *timeout;
};

#endif // VIDEOPLAYER_H
