#ifndef HUDPOWERIMPL_H
#define HUDPOWERIMPL_H

#include <QQuickPaintedItem>
#include <QQuickItem>
#include <QPainter>
#include <QTimerEvent>

#include "soro_core/constants.h"

namespace Soro {

class HudPowerImpl: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int wheelFLPower READ wheelFLPower WRITE setWheelFLPower)
    Q_PROPERTY(int wheelFRPower READ wheelFRPower WRITE setWheelFRPower)
    Q_PROPERTY(int wheelMLPower READ wheelMLPower WRITE setWheelMLPower)
    Q_PROPERTY(int wheelMRPower READ wheelMRPower WRITE setWheelMRPower)
    Q_PROPERTY(int wheelBLPower READ wheelBLPower WRITE setWheelBLPower)
    Q_PROPERTY(int wheelBRPower READ wheelBRPower WRITE setWheelBRPower)
    Q_PROPERTY(int baselinePower READ baselinePower WRITE setBaselinePower)
    Q_PROPERTY(int criticalPower READ criticalPower WRITE setCriticalPower)
    Q_PROPERTY(float wheelColorSaturation READ wheelColorSaturation WRITE setWheelColorSaturation)
    Q_PROPERTY(float wheelColorValue READ wheelColorValue WRITE setWheelColorValue)
    Q_PROPERTY(float wheelColorBaselineHue READ wheelColorBaselineHue WRITE setWheelColorBaselineHue)
    Q_PROPERTY(float wheelColorCriticalHue READ wheelColorCriticalHue WRITE setWheelColorCriticalHue)
    Q_PROPERTY(int valueLifetime READ valueLifetime WRITE setValueLifetime)

public:
    HudPowerImpl(QQuickItem *parent=0);

    int wheelFLPower() const;
    void setWheelFLPower(int power);

    int wheelFRPower() const;
    void setWheelFRPower(int power);

    int wheelMLPower() const;
    void setWheelMLPower(int power);

    int wheelMRPower() const;
    void setWheelMRPower(int power);

    int wheelBLPower() const;
    void setWheelBLPower(int power);

    int wheelBRPower() const;
    void setWheelBRPower(int power);

    int baselinePower() const;
    void setBaselinePower(int power);

    int criticalPower() const;
    void setCriticalPower(int power);

    float wheelColorSaturation() const;
    void setWheelColorSaturation(float saturation);

    float wheelColorValue() const;
    void setWheelColorValue(float value);

    float wheelColorCriticalHue() const;
    void setWheelColorCriticalHue(float saturation);

    float wheelColorBaselineHue() const;
    void setWheelColorBaselineHue(float saturation);

    int valueLifetime() const;
    void setValueLifetime(int lifetime);

    void paint(QPainter *painter);

protected:
    void timerEvent(QTimerEvent *e);

private:
    int _wheelFLPower = 0;
    int _wheelFRPower = 0;
    int _wheelMLPower = 0;
    int _wheelMRPower = 0;
    int _wheelBLPower = 0;
    int _wheelBRPower = 0;

    int _valueLifetime = 1000;

    int _baselinePower = 150;
    int _criticalPower = 500;

    float _wheelColorSaturation = 0.89803;
    float _wheelColorValue = 0.86666;
    float _wheelColorBaselineHue = 0.26666;
    float _wheelColorCriticalHue = 0.00000;

    qint64 _wheelFLTime;
    qint64 _wheelFRTime;
    qint64 _wheelMLTime;
    qint64 _wheelMRTime;
    qint64 _wheelBLTime;
    qint64 _wheelBRTime;

    int _checkValueLifeTimerId = TIMER_INACTIVE;

    float clamp(float value);

};

} // namespace Soro

#endif // HUDPOWERIMPL_H
