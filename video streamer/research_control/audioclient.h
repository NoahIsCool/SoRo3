#ifndef SORO_AUDIOCLIENT_H
#define SORO_AUDIOCLIENT_H

#include <QObject>
#include <QDataStream>

#include "soro_core/gstreamerutil.h"
#include "soro_core/socketaddress.h"

#include "mediaclient.h"

namespace Soro {

/* Receives an audio stream from an AudioServer by
 * implementing MediaClient
 */
class AudioClient : public MediaClient {
    Q_OBJECT

public:
    explicit AudioClient(int mediaId, SocketAddress server, QHostAddress host, QObject *parent = 0);

    GStreamerUtil::AudioProfile getAudioProfile() const;

private:
    GStreamerUtil::AudioProfile _profile;

protected:
    void onServerStreamingMessageInternal(QDataStream& stream) Q_DECL_OVERRIDE;
    void onServerStartMessageInternal() Q_DECL_OVERRIDE;
    void onServerEosMessageInternal() Q_DECL_OVERRIDE;
    void onServerErrorMessageInternal() Q_DECL_OVERRIDE;
    void onServerConnectedInternal();
    void onServerDisconnectedInternal();
};

} // namespace Soro

#endif // SORO_AUDIOCLIENT_H
