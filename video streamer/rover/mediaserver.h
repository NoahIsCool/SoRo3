#ifndef SORO_ROVER_MEDIASERVER_H
#define SORO_ROVER_MEDIASERVER_H

#include <QObject>
#include <QUdpSocket>
#include <QProcess>

#include "soro_core/socketaddress.h"
#include "soro_core/channel.h"

namespace Soro {

/**
 * Abstract class implementing the base functionality for sending a UDP media
 * stream to a MediaClient. This class runs in the main process, and controls a corresponding
 * MediaStreamer instance in a child process. It is done this way to prevent any encoding errors
 * from crashing the main program.
 */
class MediaServer: public QObject {
    Q_OBJECT
public:
    enum State {
        IdleState,
        /* The server is configured for a video stream and is waiting for a client
         * to connect so it can receive the stream. No pipeline or stream will
         * be created until after a client is successfully connected.
         */
        WaitingState,

        /* The server is configured and currently streaming video to a client.
         */
        StreamingState
    };

    ~MediaServer();

    /**
     * Stops the media stream. This will immediately put the server into the Idle state. If the server
     * is already stopped, nothing will happen.
     */
    void stop();

    /**
     * Gets the media ID associated with this stream.
     */
    int getMediaId();

    /**
     * Gets the state the media stream is currently in.
     */
    MediaServer::State getState() const;

private:
    int _mediaId;
    quint16 _bindPort;
    Channel *_controlChannel = nullptr;
    QUdpSocket *_mediaSocket = nullptr;
    State _state = IdleState;
    QProcess _child;
    QTcpServer *_ipcServer = nullptr;
    QTcpSocket *_ipcSocket = nullptr;
    int _startInternalTimerId = TIMER_INACTIVE;

    void beginStream(SocketAddress address);

    /**
     * Internal state change method
     */
    void setState(MediaServer::State state);

private Q_SLOTS:
    void mediaSocketReadyRead();
    void controlChannelStateChanged(Channel::State state);
    void beginClientHandshake();
    void childStateChanged(QProcess::ProcessState state);
    void ipcServerClientAvailable();

Q_SIGNALS:
    void stateChanged(MediaServer *server, MediaServer::State state);
    void eos(MediaServer *server);
    /**
     * Signal emitted when the streaming process encounters a fatal error.
     * @param server
     * @param message
     */
    void error(MediaServer *server, QString message);

protected:
    QString LOG_TAG;

    /**
     * @param logTag Tag that will be used for logging info.
     * @param mediaId Used to identify this particular media stream. Must match on both ends, and should be unique across
     * all media streams to prevent any problems if ports are not properly configured.
     * @param childProcessPath The path to the process that will be executed to provide the media stream. This is done
     * in a child process to prevent any streaming/encoding errors from crashing the main program.
     * @param bindPort Port for the local socket to bind to
     * @param parent
     */
    MediaServer(QString logTag, int mediaId, QString childProcessPath, quint16 bindPort, QObject *parent);

    /**
     * Starts the media stream. This will immediately put the media server into the Waiting state. If
     * the server is already streaming, it will be stopped and then restarted in case the configuration has changed.
     */
    void initStream();

    virtual void onStreamStoppedInternal() = 0;
    virtual void constructChildArguments(QStringList& outArgs, quint16 bindPort, SocketAddress address, quint16 ipcPort)=0;
    virtual void constructStreamingMessage(QDataStream& stream)=0;

};

} // namespace Soro

#endif // SORO_ROVER_MEDIASERVER_H
