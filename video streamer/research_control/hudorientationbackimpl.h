#ifndef HUDORIENTATIONBACKIMPL_H
#define HUDORIENTATIONBACKIMPL_H

#include <QQuickItem>
#include <QPainter>

#include "soro_core/constants.h"
#include "abstracthudorientationimpl.h"

namespace Soro {

class HudOrientationBackImpl: public AbstractHudOrientationImpl
{
    Q_OBJECT
    Q_PROPERTY(float frontRoll READ getFrontRoll WRITE setFrontRoll)
    Q_PROPERTY(float rearRoll READ getRearRoll WRITE setRearRoll)
    Q_PROPERTY(float middleRoll READ getMiddleRoll WRITE setMiddleRoll)
    Q_PROPERTY(float frontRollZero READ getFrontRollZero WRITE setFrontRollZero)
    Q_PROPERTY(float rearRollZero READ getRearRollZero WRITE setRearRollZero)
    Q_PROPERTY(float middleRollZero READ getMiddleRollZero WRITE setMiddleRollZero)

public:
    HudOrientationBackImpl(QQuickItem *parent=0);

    void paint(QPainter *painter);

    float getRearRoll() const;
    void setRearRoll(float rearRoll);

    float getFrontRoll() const;
    void setFrontRoll(float frontRoll);

    float getMiddleRoll() const;
    void setMiddleRoll(float middleRoll);

    float getRearRollZero() const;
    void setRearRollZero(float rearRollZero);

    float getFrontRollZero() const;
    void setFrontRollZero(float frontRollZero);

    float getMiddleRollZero() const;
    void setMiddleRollZero(float middleRollZero);

private:
    float rollToDegrees(float roll, float rollZero);
    float rollToDegreesAccel(float roll, float rollZero);

    float _frontRollZero;
    float _rearRollZero;
    float _middleRollZero;

    float _frontRoll;
    float _rearRoll;
    float _middleRoll;
};

} // namespace Soro

#endif // HUDORIENTATIONBACKIMPL_H
