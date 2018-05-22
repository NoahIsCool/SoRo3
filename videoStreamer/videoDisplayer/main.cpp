#include <QCoreApplication>
#include <QMessageBox>
#include <QQuickStyle>
#include <QProcessEnvironment>
#include <QtWebEngine>
#include <Qt5GStreamer/QGst/Init>

#include "videowindow.h"
#include "mpvlauncher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    MPVLauncher launcher();

    return a.exec();
}
