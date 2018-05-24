#include <QApplication>
#include <QProcessEnvironment>
#include <Qt5GStreamer/QGst/Init>
#include <iostream>

#include "player.h"
#include "mpvlauncher.h"

int main(int argc, char *argv[])
{
    QGst::init();
    QApplication a(argc, argv);

    std::cout << "calling launcher" << std::endl;
    MPVLauncher launcher(&a);
    launcher.start();

    return a.exec();
}
