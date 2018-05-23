#include <QCoreApplication>
#include <QProcessEnvironment>
#include <Qt5GStreamer/QGst/Init>
#include <iostream>

#include "videowindow.h"
#include "mpvlauncher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    std::cout << "calling launcher" << std::endl;
    MPVLauncher launcher(0);
    launcher.start();

    return a.exec();
}
