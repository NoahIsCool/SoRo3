#ifndef QMLGSTREAMERPAINTEDITEM_H
#define QMLGSTREAMERPAINTEDITEM_H

#include <QQuickPaintedItem>
#include <Qt5GStreamer/QGst/Element>

namespace Soro {

/* QML item that can display GStreamer video.
 *
 * This item is painted using the QPainter API, and uses
 * a qt5videosink element to render the video. Therefore, this
 * uses software rendering and can potentially slow down the UI.
 *
 * This item should only be used in the event QmlGStreamerGlItem
 * does not work properly on a particular system.
 */
class QmlGStreamerPaintedItem : public QQuickPaintedItem
{
    Q_OBJECT

public:
    QmlGStreamerPaintedItem(QQuickItem *parent=0);
    ~QmlGStreamerPaintedItem();

    void paint(QPainter *painter) override;

    /* Gets the video sink used by the item. This will create
     * the sink the first time it is called. You can add this sink
     * to your pipeline to display video on this item.
     */
    QGst::ElementPtr videoSink();

protected:
    void onUpdate();

private:
    QGst::ElementPtr _sink;
};

} // namespace Soro

#endif // QMLGSTREAMERPAINTEDITEM_H
