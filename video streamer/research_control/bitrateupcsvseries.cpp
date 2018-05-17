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

#include "bitrateupcsvseries.h"

namespace Soro {

BitrateUpCsvSeries::BitrateUpCsvSeries(QObject *parent) : CsvDataSeries(parent) { }

QString BitrateUpCsvSeries::getSeriesName() const
{
    return "Bitrate From Rover (b/s)";
}

void BitrateUpCsvSeries::bitrateUpdate(int bpsUp)
{
    update(QVariant(bpsUp));
}

bool BitrateUpCsvSeries::shouldKeepOldValues() const
{
    return true;
}

} // namespace Soro
