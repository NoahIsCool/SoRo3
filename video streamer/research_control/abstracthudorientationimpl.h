#ifndef ABSTRACTHUDORIENTATIONIMPL_H
#define ABSTRACTHUDORIENTATIONIMPL_H

#include <QQuickPaintedItem>
#include <QQuickItem>

namespace Soro {

class AbstractHudOrientationImpl: public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int rearYaw READ rearYaw WRITE setRearYaw)
    Q_PROPERTY(int rearPitch READ rearPitch WRITE setRearPitch)
    Q_PROPERTY(int rearRoll READ rearRoll WRITE setRearRoll)
    Q_PROPERTY(int frontYaw READ frontYaw WRITE setFrontYaw)
    Q_PROPERTY(int frontPitch READ frontPitch WRITE setFrontPitch)
    Q_PROPERTY(int frontRoll READ frontRoll WRITE setFrontRoll)

public:
    AbstractHudOrientationImpl(QQuickItem *parent=0);

    int rearYaw() const;
    void setRearYaw(int yaw);

    int rearPitch() const;
    void setRearPitch(int pitch);

    int rearRoll() const;
    void setRearRoll(int roll);

    int frontYaw() const;
    void setFrontYaw(int yaw);

    int frontPitch() const;
    void setFrontPitch(int pitch);

    int frontRoll() const;
    void setFrontRoll(int roll);

protected:
    int _rearYaw;
    int _rearPitch;
    int _rearRoll;
    int _frontYaw;
    int _frontPitch;
    int _frontRoll;
};

} // namespace Soro

#endif // ABSTRACTHUDORIENTATIONIMPL_H
