#ifndef QMLGSTREAMERGLITEM_H
#define QMLGSTREAMERGLITEM_H

#include <QQuickItem>
#include <Qt5GStreamer/QGst/Element>

namespace Soro {

/* QML item that can display GStreamer video.
 *
 * This item is painted on the scene graph by a
 * qtquick2videosink element. Therefore, it uses OpenGL
 * rendering and is more efficient than QmlGStreamerPaintedItem.
 */
class QmlGStreamerGlItem : public QQuickItem
{
    Q_OBJECT

public:
    QmlGStreamerGlItem(QQuickItem *parent=0);
    ~QmlGStreamerGlItem();

    /* Gets the video sink used by the item. This will create
     * the sink the first time it is called. You can add this sink
     * to your pipeline to display video on this item.
     */
    QGst::ElementPtr videoSink();

protected:
    QSGNode* updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
    void onUpdate();

private:
    QGst::ElementPtr _sink;
    QRectF _targetArea;
    bool _surfaceDirty;
};

} // namespace Soro

#endif // QMLGSTREAMERGLITEM_H
