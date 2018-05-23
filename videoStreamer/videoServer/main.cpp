#include <QCoreApplication>
#include <thread>

#include "videostreamer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VideoStreamer streamer(QString("/home/soro/videoStreamer/config/videoStreamer.conf"));

    return a.exec();
}
