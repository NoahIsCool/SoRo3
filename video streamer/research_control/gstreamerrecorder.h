#ifndef GSTREAMERRECORDER_H
#define GSTREAMERRECORDER_H

#include <QObject>
#include <QDateTime>

#include "soro_core/gstreamerutil.h"
#include "soro_core/socketaddress.h"

#include <Qt5GStreamer/QGst/Pipeline>
#include <Qt5GStreamer/QGst/Message>
#include <Qt5GStreamer/QGst/Bin>

namespace Soro {

class GStreamerRecorder : public QObject
{
    Q_OBJECT
public:
    explicit GStreamerRecorder(SocketAddress mediaAddress, QString name, QObject *parent=0);

    bool begin(quint8 codec, QDateTime startTime, bool vaapiEncode);
    void stop();

private Q_SLOTS:
    void onBusMessage(const QGst::MessagePtr & message);

private:
    QGst::PipelinePtr _pipeline;
    QGst::BinPtr _bin;
    QString _name;
    SocketAddress _mediaAddress;

};

} // namespace Soro

#endif // GSTREAMERRECORDER_H
