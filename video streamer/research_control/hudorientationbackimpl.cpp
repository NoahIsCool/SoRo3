/*
 * Copyright 2017 The University of Oklahoma.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hudorientationbackimpl.h"

namespace Soro {

HudOrientationBackImpl::HudOrientationBackImpl(QQuickItem *parent): AbstractHudOrientationImpl(parent)
{
    _frontRoll = _frontRollZero = 500;
    _rearRoll = _rearRollZero = 500;
    _middleRoll = _middleRollZero = 500;
}

float HudOrientationBackImpl::rollToDegrees(float roll, float rollZero)
{
    return 1.5 * -((roll - (rollZero - 500) - 100.0) * (180.0/800.0) - 90.0);
}

float HudOrientationBackImpl::rollToDegreesAccel(float roll, float rollZero)
{
    return 4* -((roll - (rollZero - 500) - 100.0) * (180.0/800.0) - 90.0);
}

void HudOrientationBackImpl::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);


    //
    // Draw FRONT wheels
    //
    painter->resetTransform();
    painter->translate(width() / 2, height() / 2);
    painter->rotate(rollToDegrees(_frontRoll, _frontRollZero));

    QPen pen3;
    pen3.setColor(QColor("#d50000"));
    pen3.setWidth(width() / 20);
    painter->setPen(pen3);

    // Draw axel
    QPainterPath axelPath3;
    axelPath3.moveTo(width() * 2 / 5 - pen3.width() - (width() / 2), 0);
    axelPath3.lineTo((width() / 2) - width() * 2 / 5 + pen3.width(), 0);
    painter->drawPath(axelPath3);

    // Draw left wheel
    QPainterPath lPath3;
    lPath3.moveTo(-width() / 2, height() * 4 / 15 - (height() / 2));
    lPath3.lineTo(width() / 5 - (width() / 2), height() * 4 / 15 - (height() / 2));
    lPath3.lineTo(width() * 2 / 5 - (width() / 2), 0);
    lPath3.lineTo(width() / 5 - (width() / 2), height() * 11 / 15 - (height() / 2));
    lPath3.lineTo(-width() / 2, height() * 11 / 15 - (height() / 2));
    lPath3.closeSubpath();
    painter->fillPath(lPath3, QBrush(QColor("#d50000")));

    // Draw right wheel
    QPainterPath rPath3;
    rPath3.moveTo(width() / 2, height() * 4 / 15 - (height() / 2));
    rPath3.lineTo(width() / 2 - width() / 5, height() * 4 / 15 - (height() / 2));
    rPath3.lineTo(width() / 2 - width() * 2 / 5, 0);
    rPath3.lineTo(width() / 2 - width() / 5, height() * 11 / 15 - (height() / 2));
    rPath3.lineTo(width() / 2, height()  * 11 / 15 - (height() / 2));
    rPath3.closeSubpath();
    painter->fillPath(rPath3, QBrush(QColor("#d50000")));

    //
    // Draw MIDDLE wheels
    //
    painter->resetTransform();
    painter->translate(width() / 2, height() / 2);
    painter->rotate(rollToDegrees(_middleRoll, _middleRollZero));

    QPen pen2;
    pen2.setColor(QColor("#00C853"));
    pen2.setWidth(width() / 20);
    painter->setPen(pen2);

    // Draw axel
    QPainterPath axelPath2;
    axelPath2.moveTo(width() * 2 / 5 - pen2.width() - (width() / 2), 0);
    axelPath2.lineTo((width() / 2) - width() * 2 / 5 + pen2.width(), 0);
    painter->drawPath(axelPath2);

    // Draw mast
    QPainterPath mastPath;
    mastPath.moveTo(0, 0);
    mastPath.lineTo(0, -height() / 2);
    painter->drawPath(mastPath);

    // Draw left wheel
    QPainterPath lPath2;
    lPath2.moveTo(-width() / 2, height() * 4 / 15 - (height() / 2));
    lPath2.lineTo(width() / 5 - (width() / 2), height() * 4 / 15 - (height() / 2));
    lPath2.lineTo(width() * 2 / 5 - (width() / 2), 0);
    lPath2.lineTo(width() / 5 - (width() / 2), height() * 11 / 15 - (height() / 2));
    lPath2.lineTo(-width() / 2, height() * 11 / 15 - (height() / 2));
    lPath2.closeSubpath();
    painter->fillPath(lPath2, QBrush(QColor("#00C853")));

    // Draw right wheel
    QPainterPath rPath2;
    rPath2.moveTo(width() / 2, height() * 4 / 15 - (height() / 2));
    rPath2.lineTo(width() / 2 - width() / 5, height() * 4 / 15 - (height() / 2));
    rPath2.lineTo(width() / 2 - width() * 2 / 5, 0);
    rPath2.lineTo(width() / 2 - width() / 5, height() * 11 / 15 - (height() / 2));
    rPath2.lineTo(width() / 2, height()  * 11 / 15 - (height() / 2));
    rPath2.closeSubpath();
    painter->fillPath(rPath2, QBrush(QColor("#00C853")));




    //
    // Draw BACK wheels
    //
    painter->resetTransform();
    painter->translate(width() / 2, height() / 2);
    painter->rotate(rollToDegreesAccel(_rearRoll, _rearRollZero));  //_middleRoll, _middleRollZero

    QPen pen;
    pen.setColor(QColor("#2962FF"));
    pen.setWidth(width() / 20);
    painter->setPen(pen);

    // Draw axel
    QPainterPath axelPath;
    axelPath.moveTo(width() * 2 / 5 - pen.width() - (width() / 2), 0);
    axelPath.lineTo((width() / 2) - width() * 2 / 5 + pen.width(), 0);
    painter->drawPath(axelPath);

    // Draw left wheel
    QPainterPath lPath;
    lPath.moveTo(-width() / 2, height() * 4 / 15 - (height() / 2));
    lPath.lineTo(width() / 5 - (width() / 2), height() * 4 / 15 - (height() / 2));
    lPath.lineTo(width() * 2 / 5 - (width() / 2), 0);
    lPath.lineTo(width() / 5 - (width() / 2), height() * 11 / 15 - (height() / 2));
    lPath.lineTo(-width() / 2, height() * 11 / 15 - (height() / 2));
    lPath.closeSubpath();
    painter->fillPath(lPath, QBrush(QColor("#2962FF")));

    // Draw right wheel
    QPainterPath rPath;
    rPath.moveTo(width() / 2, height() * 4 / 15 - (height() / 2));
    rPath.lineTo(width() / 2 - width() / 5, height() * 4 / 15 - (height() / 2));
    rPath.lineTo(width() / 2 - width() * 2 / 5, 0);
    rPath.lineTo(width() / 2 - width() / 5, height() * 11 / 15 - (height() / 2));
    rPath.lineTo(width() / 2, height()  * 11 / 15 - (height() / 2));
    rPath.closeSubpath();
    painter->fillPath(rPath, QBrush(QColor("#2962FF")));
}

float HudOrientationBackImpl::getRearRoll() const
{
    return _rearRoll;
}

void HudOrientationBackImpl::setRearRoll(float rearRoll)
{
    _rearRoll = rearRoll;
    update();
}

float HudOrientationBackImpl::getFrontRoll() const
{
    return _frontRoll;
}

void HudOrientationBackImpl::setFrontRoll(float frontRoll)
{
    _frontRoll = frontRoll;
    update();
}

float HudOrientationBackImpl::getMiddleRoll() const
{
    return _middleRoll;
}

void HudOrientationBackImpl::setMiddleRoll(float middleRoll)
{
    _middleRoll = middleRoll;
    update();
}

float HudOrientationBackImpl::getRearRollZero() const
{
    return _rearRollZero;
}

void HudOrientationBackImpl::setRearRollZero(float rearRollZero)
{
    _rearRollZero = rearRollZero;
    update();
}

float HudOrientationBackImpl::getFrontRollZero() const
{
    return _frontRollZero;
}

void HudOrientationBackImpl::setFrontRollZero(float frontRollZero)
{
    _frontRollZero = frontRollZero;
    update();
}

float HudOrientationBackImpl::getMiddleRollZero() const
{
    return _middleRollZero;
}

void HudOrientationBackImpl::setMiddleRollZero(float middleRollZero)
{
    _middleRollZero = middleRollZero;
    update();
}

} // namespace Soro
