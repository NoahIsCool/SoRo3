#ifndef SORO_VIDEOCLIENT_H
#define SORO_VIDEOCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QList>

#include "soro_core/channel.h"
#include "soro_core/gstreamerutil.h"
#include "soro_core/socketaddress.h"

#include "mediaclient.h"

namespace Soro {

/* Receives a video stream from a VideoServer by
 * implementing MediaClient
 */
class VideoClient : public MediaClient {
    Q_OBJECT

public:
    explicit VideoClient(int mediaId, SocketAddress server, QHostAddress host, QObject *parent = 0);

    GStreamerUtil::VideoProfile getVideoProfile() const;
    bool getIsStereo() const;

private:
    GStreamerUtil::VideoProfile _profile;
    bool _stereo = false;

protected:
    void onServerStreamingMessageInternal(QDataStream& stream) Q_DECL_OVERRIDE;
    void onServerStartMessageInternal() Q_DECL_OVERRIDE;
    void onServerEosMessageInternal() Q_DECL_OVERRIDE;
    void onServerErrorMessageInternal() Q_DECL_OVERRIDE;
    void onServerConnectedInternal() Q_DECL_OVERRIDE;
    void onServerDisconnectedInternal() Q_DECL_OVERRIDE;
};

} // namespace Soro

#endif // SORO_VIDEOCLIENT_H
