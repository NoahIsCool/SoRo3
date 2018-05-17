#ifndef SORO_MEDIACLIENT_H
#define SORO_MEDIACLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QDataStream>
#include <QByteArray>
#include <QList>

#include "soro_core/channel.h"
#include "soro_core/socketaddress.h"
#include "soro_core/gstreamerutil.h"

namespace Soro {

/* Abstract class implementing the base functionality
 * for receiving a UDP stream from a MediaServer.
 */
class MediaClient : public QObject {
    Q_OBJECT
public:
    enum State {
        ConnectingState,
        ConnectedState,
        StreamingState
    };

    ~MediaClient();

    void addForwardingAddress(SocketAddress address);
    void removeForwardingAddress(SocketAddress address);

    SocketAddress getServerAddress() const;
    SocketAddress getHostAddress() const;
    MediaClient::State getState() const;
    int getMediaId() const;
    QString getErrorString() const;
    int getBitrate() const;

Q_SIGNALS:
    void stateChanged(MediaClient *client, MediaClient::State state);
    void nameChanged(MediaClient *client, QString name);

private:
    QString LOG_TAG;
    char *_buffer;
    int _mediaId;
    bool _needsData = true;
    SocketAddress _server;
    State _state = ConnectingState;
    QUdpSocket *_mediaSocket;
    Channel *_controlChannel;
    int _punchTimerId = TIMER_INACTIVE;
    int _calculateBitrateTimerId = TIMER_INACTIVE;
    QList<SocketAddress> _forwardAddresses;
    long _bitCount = 0;
    int _lastBitrate = 0;
    QString _errorString = "";

    void setState(State state);
    void setCameraName(QString name);

private Q_SLOTS:
    void controlMessageReceived(const char *message, Channel::MessageSize size);
    void mediaSocketReadyRead();
    void controlChannelStateChanged(Channel::State state);

protected:
    void timerEvent(QTimerEvent *e);

    MediaClient(QString logTag, int mediaId, SocketAddress server, QHostAddress host, QObject *parent = 0);

    virtual void onServerStreamingMessageInternal(QDataStream& stream)=0;
    virtual void onServerStartMessageInternal()=0;
    virtual void onServerEosMessageInternal()=0;
    virtual void onServerErrorMessageInternal()=0;
    virtual void onServerConnectedInternal()=0;
    virtual void onServerDisconnectedInternal()=0;
};

} // namespace Soro

#endif // SORO_MEDIACLIENT_H
