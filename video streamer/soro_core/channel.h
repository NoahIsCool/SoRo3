#ifndef SORO_CHANNEL_H
#define SORO_CHANNEL_H

#include <QtNetwork>
#include <QQueue>

#include "soro_core_global.h"
#include "constants.h"
#include "socketaddress.h"

namespace Soro {

/* The Channel class is the core networking component in the Sooner Rover project.
 *
 * Channels abstract over message-based internet communication in a super easy way,
 * supporting either TCP or UDP as the transport protocol.
 *
 * The following information describes the inner workings of this class and is not
 * necessary to understand the API.
 *
 * The following headers are added to messages sent through a channel:
 *
 * -------- TCP message structure --------
 *  (2 bytes)   Packet Length                   (quint16)
 *  (1 byte)    Type                            (quint8)
 *  (4 bytes)   ID                              (quint32)
 *  ~ Message Data ~
 *
 * -------- UDP message structure --------
 *  (1 byte)    Type                            (quint8)
 *  (4 bytes)   ID                              (quint32)
 *  ~ Message Data ~
 *
 * The 'Type' field contains information about the data contained in the message.
 *  - For a normal message, 'Type' is TYPE_NORMAL.
 *
 * The ID field uniquely identifies all messages sent by this endpoint. The ID value
 * increases (newer messages have higher IDs), so they also function an a sequence number
 * in UDP mode.
 */
class SORO_CORE_EXPORT Channel: public QObject {
    Q_OBJECT
public:
    //data types used for header information
    typedef quint32 MessageID;     //4 bytes, unsigned 32-bit int
    typedef quint8 MessageType;    //1 byte, unsigned byte
    typedef quint16 MessageSize; //2 bytes, unsigned 16-bit int

private:
    //Message type identifiers
    static const MessageType MSGTYPE_NORMAL = 0;
    static const MessageType MSGTYPE_CLIENT_HANDSHAKE = 1;
    static const MessageType MSGTYPE_SERVER_HANDSHAKE = 2;
    static const MessageType MSGTYPE_HEARTBEAT = 3;
    static const MessageType MSGTYPE_ACK = 4;

    static const MessageSize TCP_HEADER_SIZE = sizeof(MessageSize) + sizeof(MessageID) + 1;
    static const MessageSize UDP_HEADER_SIZE = sizeof(MessageID) + 1;

    /* Private constructor */
    Channel(QObject *parent);

public:

    /* Protocol modes supported by a channel
     */
    enum Protocol {
        UdpProtocol, TcpProtocol
    };

    /* Lists the state a channel can be in
     */
    enum State {

        ReadyState, //The channel is ready to be connected as soon as open() is called

        ConnectingState,    //The channel is attempting to connect

        ConnectedState, //The channel is connected to the other side

        ErrorState  //The channel has encountered an unrecoverable error and cannot be used
                    //This is usually do to invalid configuration (specifying an unbindable port or host)
    };

    //The maximum size of a sent message (the header may make the actual message
    //slighty larger)
    static const MessageSize MAX_MESSAGE_LENGTH = 500;

    /* Creates a new channel to act as the server end point for communication
     */
    static Channel* createServer(QObject *parent, quint16 port, QString name, Protocol protocol,
             QHostAddress hostAddress = QHostAddress::Any);

    /* Creates a new channel to act as the client end point for communication
     */
    static Channel* createClient(QObject *parent, SocketAddress serverAddress, QString name, Protocol protocol,
             QHostAddress hostAddress = QHostAddress::Any);

    ~Channel();

    /* Gets the name the channel was configured with
     */
    QString getName() const;

    /* Gets the protocol the channel was configured with
     */
    Channel::Protocol getProtocol() const;

    /* Gets the address of the currently connected peer
     */
    SocketAddress getPeerAddress() const;

    /* Opens the channel and attempts to connect. This may not have an
     * immediate effect if the channel is receiving its configuration from
     * a network resource
     */
    void open();

    /* Closes communication until Open() is called again
     */
    void close();

    /* Sends a message to the other side of the channel
     */
    bool sendMessage(const char *message, Channel::MessageSize size);

    inline bool sendMessage(const QByteArray& message) {
        return sendMessage(message.constData(), message.size());
    }

    /* Returns true if this channel object acts as the server side
     */
    bool isServer() const;

    /* Gets the state the channel is currently in
     */
    Channel::State getState() const;

    /* Gets the uptime of the channel connection in seconds. Returns -1 if no
     * connection has been established
     */
    int getConnectionUptime() const;

    /* Gets the last calculated round trip time for the connection
     */
    int getLastRtt() const;

    /* Gets the number of messages send through this connection
     */
    quint64 getConnectionMessagesUp() const;

    /* Gets the number of messages received through this connection
     */
    quint64 getConnectionMessagesDown() const;

    int getBitsPerSecondUp() const;

    int getBitsPerSecondDown() const;

    int getUdpDroppedPacketsPercent();

    SocketAddress getHostAddress() const;

    SocketAddress getProvidedServerAddress() const;

    void setSendAcks(bool sendAcks);

    void setUdpDropOldPackets(bool dropOldPackets);

    void setLowDelaySocketOption(bool lowDelay);

    /* Returns true if this channel is or was connected to a peer
     * at some point
     */
    bool wasConnected() const;

    void setSimulatedDelay(int ms);

private:
    // Struct to hold packet information for delayed sending
    struct PacketWrapper {
        char *data;
        qint64 len;
    };

    char _receiveBuffer[1024];  //buffer for received messages
    char _sendBuffer[1024]; //buffer for constructing messages to send
    MessageSize _receiveBufferLength; //length of currently stored data in the receive buffer

    QString _name;  //The name of the channel, also as a UTF8 byte array for handshaking
    char *_nameUtf8;
    int _nameUtf8Size;

    State _state = ReadyState;   //current state the channel is in

    qint64 *_sentTimeLog;   //Used for statistic calculation
    int _sentTimeLogIndex;
    qint64 _connectionEstablishedTime;
    int _lastRtt;

    QString LOG_TAG = "CHANNEL";    //Tag for debugging, ususally the
                                     //channel name plus (S) for server or (C) for client

    QQueue<PacketWrapper*> _delayPackets;
    int _simulatedDelay = 0;

    SocketAddress _serverAddress = SocketAddress(QHostAddress::Null, 0);   //address of the server side of the channel
                                                                            //If we are the server, this may be 0 if the user
                                                                            //chose not to specify since it is not needed
    SocketAddress _hostAddress = SocketAddress(QHostAddress::Any, 0); //The address of the host to bind the channel to
    SocketAddress _peerAddress = SocketAddress(QHostAddress::Null, 0); //The address of the currently connected peer

    Protocol _protocol; //Protocol used by the channel (UDP or TCP)

    bool _isServer;         //Holders for configuration preferences
    bool _dropOldPackets = true;
    bool _sendAcks = true;
    int _lowDelaySocketOption = false;
    bool _configured = false;
    bool _wasConnected = false;

    QTcpSocket *_tcpSocket = nullptr; //Currently active TCP socket
    QTcpServer *_tcpServer = nullptr; //Currently active TCP server (for registering TCP clients)
    QUdpSocket *_udpSocket = nullptr; //Currently active UDP socket
    QAbstractSocket *_socket = nullptr;   //Pointer to either the TCP or UDP socket, depending on the configuration

    MessageID _nextSendID; //ID to mark the next message with
    MessageID _lastReceiveID;  //ID the most recent inbound message was marked with
    quint64 _messagesUp;    //Total number of sent messages
    quint64 _messagesDown;  //Total number of received messages
    quint64 _bytesUp;
    quint64 _bytesDown;
    int _droppedPackets;
    int _receivedPackets;
    int _dataRateUp;
    int _dataRateDown;

    int _connectionMonitorTimerID = TIMER_INACTIVE;  //Timer ID's for repeatedly executed tasks and watchdogs
    int _handshakeTimerID = TIMER_INACTIVE;
    int _resetTimerID = TIMER_INACTIVE;
    int _resetTcpTimerID = TIMER_INACTIVE;

    qint64 _lastReceiveTime = QDateTime::currentMSecsSinceEpoch(); //Last time a message was received
    qint64 _lastSendTime = QDateTime::currentMSecsSinceEpoch();
    qint64 _lastAckReceiveTime = QDateTime::currentMSecsSinceEpoch();
    qint64 _lastAckSendTime = QDateTime::currentMSecsSinceEpoch();

    inline void setChannelState(State state, bool forceUpdate);  //Internal method to set the channel status and
                                                          //emit the statusChanged signal

    inline void setPeerAddress(SocketAddress address); //Internal method to set the channel peer
                                                                //address and emit the peerAddressChanged signal

    inline bool sendMessage(const char *message, MessageSize size, MessageType type);  //Internal method to send a message with
                                                                            //a specific type field

    bool sendMessage(const char *message, MessageSize size, MessageType type, MessageID ID);  //Internal method to send a message with
                                                                                    //a specific type field and ID field

    void close(State closeState);   //Internal method to close the channel and set the closed state

    inline bool compareHandshake(const char *message, MessageSize size) const;  //Compares a received handshake message with the correct one

    void processBufferedMessage(MessageType type, MessageID ID,
                                const char *message, MessageSize size, const SocketAddress &address);   //Processes a received message

    void configureNewTcpSocket();   //Sets up a newly created TCP socket

    void resetConnectionVars(); //Resets variables relating to the current connection state,
                                //called when a new connection is established

    inline void initVars(); //Initializes variables when the channel is fist created (mostly nulling pointers)

    void init();

    void resetConnection(); //Closes any existing connections and attempts to reconnect. Called in the event
                            //of a network error or dropped connection. Also called for initial connection.

    inline void sendHandshake();    //Sends a handshake message to the connected peer

    inline void sendHeartbeat();    //Sends a heartbeat message to the connected peer

    void setName(QString name); //sets the name of the channel

private Q_SLOTS:
    void udpReadyRead();
    void tcpReadyRead();
    void tcpConnected();
    void newTcpClient();
    void connectionErrorInternal(QAbstractSocket::SocketError err);
    void serverErrorInternal(QAbstractSocket::SocketError err);

Q_SIGNALS:

    /* Signal to notify an observer that a message has been received
     */
    void messageReceived(const char *message, Channel::MessageSize size);

    /* Signal to notify an observer that the state of the channel has changed
     */
    void stateChanged(Channel::State state);

    /* Signal to notify an observer that the connected peer has changed
     * If the channel is no longer connected, peerAddress.address will be QHostAddress::Null
     *
     * Also, if this channel is the client side of a UDP connection, peer address will always be the
     * server address regardless of the state of the connection since that is the only allowed address
     * for communication.
     */
    void peerAddressChanged(const SocketAddress &peerAddress);

    void connectionError(QAbstractSocket::SocketError err);

    void rttChanged(int rtt);

protected:
    void timerEvent(QTimerEvent *);

};

}

#endif // SORO_CHANNEL_H
