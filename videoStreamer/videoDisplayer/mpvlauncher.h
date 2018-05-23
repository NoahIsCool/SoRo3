#ifndef MPVLAUNCHER_H
#define MPVLAUNCHER_H
#include <QTextStream>
#include <QTimer>
#include <QHostAddress>
#include <QObject>
#include <thread>

#include "socket.h"
#include "soro_global.h"
#include "configreader.h"
#include "logger.h"

using namespace soro;

class MPVLauncher : public QObject
{
    Q_OBJECT
public:
    explicit MPVLauncher(QObject *parent = nullptr);
    void start();

signals:

public slots:
    void beatHeart();
    void checkHeartBeat(DataPacket packet);
    void onMessage(DataPacket packet);
    void attemptConnection();

private:
    const QString LOG_TAG = "Video Displayer";
    QHostAddress *rover;
    socket *control;
    socket *heartbeat;
    QTimer *heartbeatTimer;
    QTimer *connectTimer;
    QByteArray *initMessage;
    std::thread *inputThread;
    std::thread *frontVideoThread = nullptr;
    std::thread *backVideoThread = nullptr;
    std::thread *clawVideoThread = nullptr;
    bool connected;

    void processInput();
};

#endif // MPVLAUNCHER_H
