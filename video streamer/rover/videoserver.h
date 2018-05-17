#ifndef VIDEOSERVER_H
#define VIDEOSERVER_H

#include <QObject>

#include "soro_core/socketaddress.h"
#include "soro_core/gstreamerutil.h"
#include "mediaserver.h"

namespace Soro {

/**
 * Sends a video stream to a VideoClient by
 * implementing MediaServer
 */
class VideoServer: public MediaServer {
    Q_OBJECT
public:
    /**
     * @param mediaId Used to identify this particular media stream. Must match on both ends, and should be unique across
     * all media streams to prevent any problems if ports are not properly configured.
     * @param host Address for the socket that will be used to communicate with the media client.
     * @param parent
     */
    explicit VideoServer(int mediaId, quint16 bindPort, QObject *parent = 0);

    /**
     * Starts a video stream. If the server is already streaming, it will be stopped and restarted to
     * accomodate any configuration changes.
     *
     * @param deviceName The video device to connect to and start streaming (/dev/video*)
     * @param format The video format to stream.
     */
    void start(QString deviceName, GStreamerUtil::VideoProfile profile, bool vaapi);

    /**
     * Starts a stereo video stream, using two video devices. If the server is already streaming, it will be stopped and restarted to
     * accomodate any configuration changes.
     *
     * @param leftDeviceName The left video device to connect to and start streaming (/dev/video*)
     * @param rightDeviceName The right video device to connect to and start streaming (/dev/video*)
     * @param format The video format to stream.
     */
    void start(QString leftDeviceName, QString rightDeviceName, GStreamerUtil::VideoProfile profile, bool vaapi);

    GStreamerUtil::VideoProfile getVideoProfile() const;

private:
    GStreamerUtil::VideoProfile _profile;
    QString _videoDevice;
    bool _starting = false;
    bool _vaapi = false;
    bool _stereo = false;

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

#endif // VIDEOSERVER_H
