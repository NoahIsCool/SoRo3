#ifndef AUDIOSERVER_H
#define AUDIOSERVER_H

#include <QObject>

#include "soro_core/socketaddress.h"
#include "mediaserver.h"
#include "soro_core/gstreamerutil.h"

namespace Soro {

/**
 * Sends an audio stream to an AudioClient by
 * implementing MediaServer
 */
class AudioServer: public MediaServer {
    Q_OBJECT
public:
    /**
     * @param mediaId Used to identify this particular media stream. Must match on both ends, and should be unique across
     * all media streams to prevent any problems if ports are not properly configured.
     * @param host Address for the socket that will be used to communicate with the media client.
     * @param parent
     */
    explicit AudioServer(int mediaId, quint16 bindPort, QObject *parent = nullptr);

    /**
     * Starts an audio stream. If the server is already streaming, it will be stopped and restarted to
     * accomodate any configuration changes.
     *
     * @param profile The audio profile to stream.
     */
    void start(GStreamerUtil::AudioProfile profile);

    /**
     * Gets the audio profile currently being streamed. If no audio is streaming, this will return a profile with CODEC_NULL.
     */
    GStreamerUtil::AudioProfile getAudioProfile() const;

private:
    GStreamerUtil::AudioProfile _profile;
    bool _starting = false;

protected:
    /**
     * Begins streaming video to the provided address.
     * This will fail if the stream is not in WaitingState
     */
    void constructChildArguments(QStringList& outArgs, quint16 bindPort, SocketAddress address, quint16 ipcPort) Q_DECL_OVERRIDE;

    void onStreamStoppedInternal() Q_DECL_OVERRIDE;

    void constructStreamingMessage(QDataStream& stream) Q_DECL_OVERRIDE;
};

} // namespace Soro

#endif // AUDIOSERVER_H
