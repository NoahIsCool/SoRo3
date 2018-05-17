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

#include "abstracthudorientationimpl.h"

namespace Soro {

AbstractHudOrientationImpl::AbstractHudOrientationImpl(QQuickItem *parent): QQuickPaintedItem(parent)
{
}

int AbstractHudOrientationImpl::rearYaw() const
{
    return _rearYaw;
}

void AbstractHudOrientationImpl::setRearYaw(int yaw)
{
    _rearYaw = yaw;
}

int AbstractHudOrientationImpl::rearPitch() const
{
    return _rearPitch;
}

void AbstractHudOrientationImpl::setRearPitch(int pitch)
{
    _rearPitch = pitch;
}

int AbstractHudOrientationImpl::rearRoll() const
{
    return _rearRoll;
}

void AbstractHudOrientationImpl::setRearRoll(int roll)
{
    _rearRoll = roll;
}

int AbstractHudOrientationImpl::frontYaw() const
{
    return _frontYaw;
}

void AbstractHudOrientationImpl::setFrontYaw(int yaw)
{
    _frontYaw = yaw;
}

int AbstractHudOrientationImpl::frontPitch() const
{
    return _frontPitch;
}

void AbstractHudOrientationImpl::setFrontPitch(int pitch)
{
    _frontPitch = pitch;
}

int AbstractHudOrientationImpl::frontRoll() const
{
    return _frontRoll;
}

void AbstractHudOrientationImpl::setFrontRoll(int roll)
{
    _frontRoll = roll;
}

} // namespace Soro
