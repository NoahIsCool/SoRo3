#ifndef MULTISINK_H
#define MULTISINK_H
#include <iostream>
#include <QCoreApplication>
#include <Qt5GStreamer/QGlib/Error>
#include <Qt5GStreamer/QGlib/Connect>
#include <Qt5GStreamer/QGst/Init>
#include <Qt5GStreamer/QGst/Bus>
#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Parse>
#include <Qt5GStreamer/QGst/Message>
#include <Qt5GStreamer/QGst/Utils/ApplicationSink>
#include <Qt5GStreamer/QGst/Utils/ApplicationSource>

class MultiSink : public QGst::Utils::ApplicationSink
{
public:
    MultiSink(QGst::Utils::ApplicationSource *src)
        : QGst::Utils::ApplicationSink(), m_src(src) {}
protected:
    virtual void eos()
    {
        m_src->endOfStream();
    }
    virtual QGst::FlowReturn newSample()
    {
        QGst::SamplePtr sample = pullSample();
        m_src->pushBuffer(sample->buffer());
        return QGst::FlowOk;
    }
private:
    QGst::Utils::ApplicationSource *m_src;
};

#endif // MULTISINK_H
