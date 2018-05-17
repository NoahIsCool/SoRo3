/*********************************************************
 * This code can be compiled on a Qt or mbed enviornment *
 *********************************************************/

#ifndef SORO_MBEDCHANNEL_H
#define SORO_MBEDCHANNEL_H

#ifdef QT_CORE_LIB
#   include <QtCore>
#   include <QUdpSocket>
#   include <QHostAddress>
#   include "soro_core_global.h"
#   include "socketaddress.h"
#   include "logger.h"
#endif
#ifdef TARGET_LPC1768
#   include "mbed.h"
#   include "EthernetInterface.h"
#   include "UDPSocket.h"
#   include "lpc_phy.h"
#endif

#include <cstring>
#include "constants.h"

/* This class is used to facilitate communication over ethernet between an actual computer
 * and and mbed. It is designed to compile both on a Qt environment or an mbed.
 */

namespace Soro {

#ifdef QT_CORE_LIB

/* Qt implementation.
 *
 * The Qt side acts as the server and waits for messages from the mbed.
 */
class SORO_CORE_EXPORT MbedChannel: public QObject {
    Q_OBJECT

public:
    enum State {
        ConnectedState, ConnectingState
    };

    /* Gets the connection state
     */
    MbedChannel::State getState() const;

private:
    QString LOG_TAG;
    QUdpSocket *_socket;
    SocketAddress _host;
    SocketAddress _mbed;
    State _state;
    char *_buffer;
    bool _active;
    char _mbedId;
    unsigned int _lastReceiveId;
    unsigned int _nextSendId = 0;
    int _watchdogTimerId = TIMER_INACTIVE;
    int _resetConnectionTimerId = TIMER_INACTIVE;
    void setChannelState(MbedChannel::State state);

private Q_SLOTS:
    void socketError(QAbstractSocket::SocketError err);
    void socketReadyRead();
    void resetConnection();

public:
    /* Create a new mbed channel, bound to a specified socket. Note the mbedId must
     * be the same on both this side and the mbed for the connection to work.
     */
    MbedChannel(SocketAddress host, unsigned char mbedId, QObject *parent = nullptr);
    ~MbedChannel();

    /* Sends a message to the mbed
     */
    void sendMessage(const char *message, int length);

Q_SIGNALS:
    /* Emitted when we get a message from the mbed
     */
    void messageReceived(const char* message, int length);
    /* Emitted when the mbed connects or times out
     */
    void stateChanged(MbedChannel::State state);

protected:
    void timerEvent(QTimerEvent *e);

};

#endif
#ifdef TARGET_LPC1768

/* Mbed implementation.
 *
 * The Mbed acts as a client, and communicates with the server through UDP broadcasting on
 * a port devined in the configuration file server.txt, which should be on the mbed's
 * local file system and contain only the port number.
 */
class MbedChannel {
private:
    EthernetInterface *_eth;
    UDPSocket *_socket;
    Endpoint _server;
    unsigned int _serverPort;
    time_t _lastSendTime;
    time_t _lastReceiveTime;
    unsigned int _nextSendId;
    unsigned int _lastReceiveId;
    char _mbedId;
    char *_buffer;
    void (*_resetCallback)(void);

    /* Called in the event of an invalid or missing config file
     */
    void panic();
    /* Resets the mbed after calling the reset listener (if it is set)
     */
    void reset();
    /* Initializes etherent and attempts to find the server through a UDP broadcast.
     * Blocks until completion.
     */
    inline void initConnection();

    void sendMessage(char *message, int size, unsigned char type);

    inline bool isEthernetActive () {
        return (lpc_mii_read_data() & (1 << 0));
    }

public:
    /* Creates a new channel for ethernet communication.
     *
     * This will read from a file on local storage (server.txt) to
     * determine the port used for communication.
     *
     * Blocks until completion, usually for several seconds.
     */
    MbedChannel(unsigned char mbedId, unsigned int port);

    ~MbedChannel();

    /* Sets the timeout for blocking socket operations in milliseconds.
     * Does not allow no timeout or timeouts greater than IDLE_CONNECTION_TIMEOUT / 2.
     */
    void setTimeout(unsigned int millis);

    void sendMessage(char *message, int length);

    /* Sends a log message to the server.
     */
    void log(char *message);

    /* Adds a listener to be notified if the ethernet
     * becomes disconnected, which will result in the mbed
     * resetting. This gives you a chance to clean up
     * or perform any necessary operations before a reset.
     */
    void setResetListener(void (*callback)(void));

    /* Reads a pending message (if available) and stores it in outMessage.
     *
     * Returns the length of the message, or -1 if none is available or
     * an error occurred.
     */
    int read(char *outMessage, int maxLength);
};

#endif

}
#endif // SORO_MBEDCHANNEL_H
