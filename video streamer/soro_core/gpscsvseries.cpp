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

#include "gpscsvseries.h"
#include "logger.h"

namespace Soro {

GpsCsvSeries::GpsCsvSeries(QObject *parent) : QObject(parent) { }

const GpsCsvSeries::LatitudeCsvSeries* GpsCsvSeries::getLatitudeSeries() const
{
    return &_latitudeSeries;
}

const GpsCsvSeries::LongitudeCsvSeries* GpsCsvSeries::getLongitudeSeries() const
{
    return &_longitudeSeries;
}

void GpsCsvSeries::addLocation(NmeaMessage location)
{
    _latitudeSeries.update(location);
    _longitudeSeries.update(location);
    Q_EMIT locationUpdated(location);
}

} // namespace Soro
