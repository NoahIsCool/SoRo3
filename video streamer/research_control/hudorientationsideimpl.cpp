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

#include "hudorientationsideimpl.h"
#include <qmath.h>

namespace Soro {

HudOrientationSideImpl::HudOrientationSideImpl(QQuickItem *parent): AbstractHudOrientationImpl(parent)
{
    _frontPitch = _frontPitchZero = 400;
    _rearPitch = _rearPitchZero = 575;
    _middlePitch = _middlePitchZero = 400;
}

float HudOrientationSideImpl::pitchToDegrees(float pitch, float pitchZero)
{
    return 1.3 * -((pitch - (pitchZero - 500) - 100.0) * (180.0/800.0) - 90.0);
}

float HudOrientationSideImpl::degToRad(float deg)
{
    return M_PI * deg / 180;
}

void HudOrientationSideImpl::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    int wheelSize = height() / 4;

    QPen pen;
    pen.setWidth(height() / 30);

    // Transform for rear components
    painter->resetTransform();
    painter->translate(width() / 2, height() / 2);
    painter->rotate(pitchToDegrees(_middlePitch, _middlePitchZero));

    // Draw mast
    pen.setColor(QColor("#00C853"));
    QPainterPath mastPath;
    mastPath.moveTo(-width() / 2 + wheelSize / 2, 0);
    mastPath.lineTo(-width() / 2 + wheelSize / 4, -height() / 3);
    painter->strokePath(mastPath, pen);

    // Draw rear frame
    pen.setColor(QColor("#00C853"));
    QPainterPath rearFramePath;
    rearFramePath.moveTo(0, 0);
    rearFramePath.lineTo(-width() / 3 + wheelSize / 4, -height() / 6);
    rearFramePath.lineTo(-width() / 2 + wheelSize / 2, 0);
    painter->strokePath(rearFramePath, pen);

    // Draw back wheels
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor("#2962FF")));
    painter->drawEllipse(QRectF(-width() / 2, -wheelSize / 2, wheelSize, wheelSize));

    // Transform for front components
    painter->resetTransform();
    painter->translate(width() / 2, height() / 2);
    painter->rotate(pitchToDegrees(_frontPitch, _frontPitchZero));

    // Draw front frame
    pen.setColor(QColor("#d50000"));
    QPainterPath frontFramePath;
    frontFramePath.moveTo(0, 0);
    frontFramePath.lineTo(width() / 6 - wheelSize / 4, -height() / 6);
    frontFramePath.lineTo(width() / 2 - wheelSize / 2, 0);
    painter->strokePath(frontFramePath, pen);

    // Draw front wheel
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor("#d50000")));
    painter->drawEllipse(QRectF(width() / 2 - wheelSize,  -wheelSize / 2, wheelSize, wheelSize));

    // Draw middle wheel
    painter->resetTransform();
    painter->translate(width() / 2, height() / 2);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor("#00C853")));
    painter->drawEllipse(QRectF(-wheelSize / 2, -wheelSize / 2, wheelSize, wheelSize));
}

void HudOrientationSideImpl::setFrontPitch(float frontPitch)
{
    _frontPitch = frontPitch;
    update();
}

float HudOrientationSideImpl::getFrontPitch() const
{
    return _frontPitch;
}

void HudOrientationSideImpl::setRearPitch(float rearPitch)
{
    _rearPitch = rearPitch;
    update();
}

float HudOrientationSideImpl::getRearPitch() const
{
    return _rearPitch;
}

void HudOrientationSideImpl::setMiddlePitch(float middlePitch)
{
    _middlePitch = middlePitch;
    update();
}

float HudOrientationSideImpl::getMiddlePitch() const
{
    return _middlePitch;
}

void HudOrientationSideImpl::setFrontPitchZero(float frontPitchZero)
{
    _frontPitchZero = frontPitchZero;
    update();
}

float HudOrientationSideImpl::getFrontPitchZero() const
{
    return _frontPitchZero;
}

void HudOrientationSideImpl::setRearPitchZero(float rearPitchZero)
{
    _rearPitchZero = rearPitchZero;
    update();
}

float HudOrientationSideImpl::getRearPitchZero() const
{
    return _rearPitchZero;
}

void HudOrientationSideImpl::setMiddlePitchZero(float middlePitchZero)
{
    _middlePitchZero = middlePitchZero;
    update();
}

float HudOrientationSideImpl::getMiddlePitchZero() const
{
    return _middlePitchZero;
}

} // namespace Soro
