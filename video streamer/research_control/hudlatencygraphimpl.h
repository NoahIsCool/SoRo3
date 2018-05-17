#ifndef HUDLATENCYGRAPHIMPL_H
#define HUDLATENCYGRAPHIMPL_H

#include <QQuickPaintedItem>
#include <QDateTime>
#include <QMap>
#include <QQuickItem>
#include <QTimerEvent>
#include <QPainter>

#include "soro_core/constants.h"

namespace Soro {

class HudLatencyGraphImpl: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QString mode READ mode WRITE setMode)
    Q_PROPERTY(int latency READ latency WRITE setLatency)
    Q_PROPERTY(float value READ value WRITE setValue)
    Q_PROPERTY(int latencyThreshold READ latencyThreshold WRITE setLatencyThreshold)

public:
    HudLatencyGraphImpl(QQuickItem *parent=0);

    QString mode() const;
    void setMode(QString mode);

    int latency() const;
    void setLatency(int latency);

    float value() const;
    void setValue(float value);

    int latencyThreshold() const;
    void setLatencyThreshold(int threshold);

    void paint(QPainter *painter);

private:
    QString _mode;
    int _latency = 0;
    int _latencyThreshold = 0;
    float _value = 0;
    int _updateTimerId = TIMER_INACTIVE;
    QMap<qint64, float> _history;

    void timerEvent(QTimerEvent *e);
    float nearestValue(qint64 time);
};

} // namespace Soro

#endif // HUDLATENCYGRAPHIMPL_H
