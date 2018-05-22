#include <QCoreApplication>
#include <QMessageBox>
#include <QQuickStyle>
#include <QProcessEnvironment>
#include <QtWebEngine>
#include <Qt5GStreamer/QGst/Init>

#include "videowindow.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    bool useHW = true;

    qmlRegisterType<HudLatencyGraphImpl>("Soro", 1, 0, "HudLatencyGraphImpl");
    qmlRegisterType<HudPowerImpl>("Soro", 1, 0, "HudPowerImpl");
    qmlRegisterType<HudOrientationSideImpl>("Soro", 1, 0, "HudOrientationSideImpl");
    qmlRegisterType<HudOrientationBackImpl>("Soro", 1, 0, "HudOrientationBackImpl");
    if (useHW)
    {
        // Use the hardware opengl rendering surface, doesn't work on some hardware
        LOG_I(LOG_TAG, "Registering QmlGStreamerItem as GStreamerSurface...");
        qmlRegisterType<QmlGStreamerGlItem>("Soro", 1, 0, "GStreamerSurface");
    }
    else
    {
        // Use the software rendering surface, works everywhere but slower
        LOG_I(LOG_TAG, "Registering QmlGStreamerPaintedItem as GStreamerSurface...");
        qmlRegisterType<QmlGStreamerPaintedItem>("Soro", 1, 0, "GStreamerSurface");
    }

    QQmlEngine *qml = new QQmlEngine(a);
    QQuickStyle::setStyle("Material");

    VideoWindow w(qml);

    return a.exec();
}
