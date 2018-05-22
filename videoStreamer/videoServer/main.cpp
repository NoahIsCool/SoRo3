#include <QCoreApplication>
#include "videostreamer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    VideoStreamer streamer(QString("/opt/project-remoteVideo/config/videoStreamer.conf"));

    return a.exec();
}
