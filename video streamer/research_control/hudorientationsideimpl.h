#ifndef HUDORIENTATIONSIDEIMPL_H
#define HUDORIENTATIONSIDEIMPL_H

#include <QQuickItem>
#include <QPainter>

#include "soro_core/constants.h"
#include "abstracthudorientationimpl.h"

namespace Soro {

class HudOrientationSideImpl: public AbstractHudOrientationImpl
{
    Q_OBJECT
    Q_PROPERTY(float frontPitch READ getFrontPitch WRITE setFrontPitch)
    Q_PROPERTY(float rearPitch READ getRearPitch WRITE setRearPitch)
    Q_PROPERTY(float middlePitch READ getMiddlePitch WRITE setMiddlePitch)
    Q_PROPERTY(float frontPitchZero READ getFrontPitchZero WRITE setFrontPitchZero)
    Q_PROPERTY(float rearPitchZero READ getRearPitchZero WRITE setRearPitchZero)
    Q_PROPERTY(float middlePitchZero READ getMiddlePitchZero WRITE setMiddlePitchZero)

public:
    HudOrientationSideImpl(QQuickItem *parent=0);

    void paint(QPainter *painter);

    void setFrontPitch(float frontPitch);
    float getFrontPitch() const;

    void setRearPitch(float rearPitch);
    float getRearPitch() const;

    void setMiddlePitch(float middlePitch);
    float getMiddlePitch() const;

    void setFrontPitchZero(float frontPitchZero);
    float getFrontPitchZero() const;

    void setRearPitchZero(float rearPitchZero);
    float getRearPitchZero() const;

    void setMiddlePitchZero(float middlePitchZero);
    float getMiddlePitchZero() const;


private:
    float pitchToDegrees(float pitch, float pitchZero);
    float degToRad(float deg);

    float _rearPitch;
    float _frontPitch;
    float _middlePitch;

    float _rearPitchZero;
    float _frontPitchZero;
    float _middlePitchZero;
};

} // namespace Soro

#endif // HUDORIENTATIONSIDEIMPL_H
