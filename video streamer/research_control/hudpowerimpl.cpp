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

#include "hudpowerimpl.h"

#include <QDateTime>

namespace Soro {

HudPowerImpl::HudPowerImpl(QQuickItem *parent) : QQuickPaintedItem(parent)
{
    START_TIMER(_checkValueLifeTimerId, _valueLifetime / 2);
}

int HudPowerImpl::wheelFLPower() const
{
    return _wheelFLPower;
}

void HudPowerImpl::setWheelFLPower(int power)
{
    _wheelFLPower = power;
    _wheelFLTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    update();
}

int HudPowerImpl::wheelFRPower() const
{
    return _wheelFRPower;
}

void HudPowerImpl::setWheelFRPower(int power)
{
    _wheelFRPower = power;
    _wheelFRTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    update();
}

int HudPowerImpl::wheelMLPower() const
{
    return _wheelMLPower;
}

void HudPowerImpl::setWheelMLPower(int power)
{
    _wheelMLPower = power;
    _wheelMLTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    update();
}

int HudPowerImpl::wheelMRPower() const
{
    return _wheelMRPower;
}

void HudPowerImpl::setWheelMRPower(int power)
{
    _wheelMRPower = power;
    _wheelMRTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    update();
}

int HudPowerImpl::wheelBLPower() const
{
    return _wheelBLPower;
}

void HudPowerImpl::setWheelBLPower(int power)
{
    _wheelBLPower = power;
    _wheelBLTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    update();
}

int HudPowerImpl::wheelBRPower() const
{
    return _wheelBRPower;
}

void HudPowerImpl::setWheelBRPower(int power)
{
    _wheelBRPower = power;
    _wheelBRTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    update();
}

int HudPowerImpl::baselinePower() const
{
    return _baselinePower;
}

void HudPowerImpl::setBaselinePower(int power)
{
    _baselinePower = power;
    update();
}

int HudPowerImpl::criticalPower() const
{
    return _criticalPower;
}

void HudPowerImpl::setCriticalPower(int power)
{
    _criticalPower = power;
    update();
}

float HudPowerImpl::wheelColorValue() const
{
    return _wheelColorValue;
}

void HudPowerImpl::setWheelColorValue(float value)
{
    _wheelColorValue = value;
    update();
}

float HudPowerImpl::wheelColorSaturation() const
{
    return _wheelColorSaturation;
}

void HudPowerImpl::setWheelColorSaturation(float saturation)
{
    _wheelColorSaturation = saturation;
    update();
}

float HudPowerImpl::wheelColorBaselineHue() const
{
    return _wheelColorBaselineHue;
}

void HudPowerImpl::setWheelColorBaselineHue(float hue)
{
    _wheelColorBaselineHue = hue;
    update();
}

float HudPowerImpl::wheelColorCriticalHue() const
{
    return _wheelColorCriticalHue;
}

void HudPowerImpl::setWheelColorCriticalHue(float hue)
{
    _wheelColorCriticalHue = hue;
    update();
}

int HudPowerImpl::valueLifetime() const
{
    return _valueLifetime;
}

void HudPowerImpl::setValueLifetime(int lifetime)
{
    if (lifetime < 10) lifetime = 10;
    _valueLifetime = lifetime;
    KILL_TIMER(_checkValueLifeTimerId);
    START_TIMER(_checkValueLifeTimerId, _valueLifetime / 2);
}

float HudPowerImpl::clamp(float value)
{
    if (value < 0) return 0;
    if (value > 1) return 1;
    return value;
}

void HudPowerImpl::timerEvent(QTimerEvent *e)
{
    QQuickPaintedItem::timerEvent(e);
    if (e->timerId() == _checkValueLifeTimerId)
    {
        // Check for any outdated power values
        qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
        if (now - _wheelFLTime > _valueLifetime)
        {
            setWheelFLPower(0);
        }
        if (now - _wheelFRTime > _valueLifetime)
        {
            setWheelFRPower(0);
        }
        if (now - _wheelMLTime > _valueLifetime)
        {
            setWheelMLPower(0);
        }
        if (now - _wheelMRTime > _valueLifetime)
        {
            setWheelMRPower(0);
        }
        if (now - _wheelBLTime > _valueLifetime)
        {
            setWheelBLPower(0);
        }
        if (now - _wheelBRTime > _valueLifetime)
        {
            setWheelBRPower(0);
        }
    }
}

void HudPowerImpl::paint(QPainter *painter)
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(width() / 30);
    painter->setPen(pen);

    float hueRange = _wheelColorCriticalHue - _wheelColorBaselineHue;
    float powerRange = _criticalPower - _baselinePower;

    float flRatio = clamp((_wheelFLPower - _baselinePower) / powerRange);
    float frRatio = clamp((_wheelFRPower - _baselinePower) / powerRange);
    float mlRatio = clamp((_wheelMLPower - _baselinePower) / powerRange);
    float mrRatio = clamp((_wheelMRPower - _baselinePower) / powerRange);
    float blRatio = clamp((_wheelBLPower - _baselinePower) / powerRange);
    float brRatio = clamp((_wheelBRPower - _baselinePower) / powerRange);

    // Draw front axel
    QPainterPath frontAxelPath;
    frontAxelPath.moveTo(width() * 2 / 5 - pen.width(), height() / 8);
    frontAxelPath.lineTo(width() - width() * 2 / 5 + pen.width(), height() / 8);
    painter->drawPath(frontAxelPath);

    // Draw middle axel
    QPainterPath middleAxelPath;
    middleAxelPath.moveTo(width() * 2 / 5 - pen.width(), height() / 2);
    middleAxelPath.lineTo(width() - width() * 2 / 5 + pen.width(), height() / 2);
    painter->drawPath(middleAxelPath);

    // Draw back axel
    QPainterPath backAxelPath;
    backAxelPath.moveTo(width() * 2 / 5 - pen.width(), height() - height() / 8);
    backAxelPath.lineTo(width() - width() * 2 / 5 + pen.width(), height() - height() / 8);
    painter->drawPath(backAxelPath);

    // Draw frame
    QPainterPath framePath;
    framePath.moveTo(width() / 2, height() / 8);
    framePath.lineTo(width() / 2, height() - height() / 8);
    painter->drawPath(framePath);

    // Draw FL wheel
    QPainterPath flPath;
    flPath.moveTo(0, 0);
    flPath.lineTo(width() / 5, 0);
    flPath.lineTo(width() * 2 / 5, height() / 8);
    flPath.lineTo(width() / 5, height() / 4);
    flPath.lineTo(0, height() / 4);
    flPath.closeSubpath();
    painter->fillPath(flPath, QBrush(_wheelFLPower > 100 ? QColor::fromHsvF(_wheelColorBaselineHue + flRatio * hueRange, _wheelColorSaturation, _wheelColorValue) : Qt::white));

    // Draw FR wheel
    QPainterPath frPath;
    frPath.moveTo(width(), 0);
    frPath.lineTo(width() - width() / 5, 0);
    frPath.lineTo(width() - width() * 2 / 5, height() / 8);
    frPath.lineTo(width() - width() / 5, height() / 4);
    frPath.lineTo(width(), height() / 4);
    frPath.closeSubpath();
    painter->fillPath(frPath, QBrush(_wheelFRPower > 100 ? QColor::fromHsvF(_wheelColorBaselineHue + frRatio * hueRange, _wheelColorSaturation, _wheelColorValue) : Qt::white));

    // Draw ML wheel
    QPainterPath mlPath;
    mlPath.moveTo(0, height() * 3 / 8);
    mlPath.lineTo(width() / 5, height() * 3 / 8);
    mlPath.lineTo(width() * 2 / 5, height() / 2);
    mlPath.lineTo(width() / 5, height() * 5 / 8);
    mlPath.lineTo(0, height() * 5 / 8);
    mlPath.closeSubpath();
    painter->fillPath(mlPath, QBrush(_wheelMLPower > 100 ? QColor::fromHsvF(_wheelColorBaselineHue + mlRatio * hueRange, _wheelColorSaturation, _wheelColorValue) : Qt::white));

    // Draw MR wheel
    QPainterPath mrPath;
    mrPath.moveTo(width(), height() * 3 / 8);
    mrPath.lineTo(width() - width() / 5, height() * 3 / 8);
    mrPath.lineTo(width() - width() * 2 / 5, height() / 2);
    mrPath.lineTo(width() - width() / 5, height() * 5 / 8);
    mrPath.lineTo(width(), height() * 5 / 8);
    mrPath.closeSubpath();
    painter->fillPath(mrPath, QBrush(_wheelMRPower > 100 ? QColor::fromHsvF(_wheelColorBaselineHue + mrRatio * hueRange, _wheelColorSaturation, _wheelColorValue) : Qt::white));

    // Draw BL wheel
    QPainterPath blPath;
    blPath.moveTo(0, height());
    blPath.lineTo(width() / 5, height());
    blPath.lineTo(width() * 2 / 5, height() - height() / 8);
    blPath.lineTo(width() / 5, height() - height() / 4);
    blPath.lineTo(0, height() - height() / 4);
    blPath.closeSubpath();
    painter->fillPath(blPath, QBrush(_wheelBLPower > 100 ? QColor::fromHsvF(_wheelColorBaselineHue + blRatio * hueRange, _wheelColorSaturation, _wheelColorValue) : Qt::white));

    // Draw BR wheel
    QPainterPath brPath;
    brPath.moveTo(width(), height());
    brPath.lineTo(width() - width() / 5, height());
    brPath.lineTo(width() - width() * 2 / 5, height() - height() / 8);
    brPath.lineTo(width() - width() / 5, height() - height() / 4);
    brPath.lineTo(width(), height() - height() / 4);
    brPath.closeSubpath();
    painter->fillPath(brPath, QBrush(_wheelBRPower > 100 ? QColor::fromHsvF(_wheelColorBaselineHue + brRatio * hueRange, _wheelColorSaturation, _wheelColorValue) : Qt::white));
}

} // namespace Soro
