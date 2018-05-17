#ifndef COMMENTSWINDOWCONTROLLER_H
#define COMMENTSWINDOWCONTROLLER_H

#include <QObject>
#include <QQuickWindow>
#include <QQmlEngine>

#include "soro_core/enums.h"
#include "soro_core/channel.h"

namespace Soro {

class CommentsWindowController : public QObject
{
    Q_OBJECT
public:
    explicit CommentsWindowController(QQmlEngine *engine, QObject *parent = 0);

Q_SIGNALS:
    void recordButtonClicked();
    void logCommentEntered(QString comment);
    void closed();

public Q_SLOTS:
    void setRecordingState(RecordingState state);
    void setConnectionState(Channel::State state);

private:
    QQuickWindow *_window = 0;
};

} // namespace Soro

#endif // COMMENTSWINDOWCONTROLLER_H
