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

#include "sensordataparser.h"

#define LOG_TAG "SensorDataRecorder"

namespace Soro {

SensorDataParser::SensorDataParser(QObject *parent) : QObject(parent) { }

void SensorDataParser::newData(const char* data, int len)
{
    _buffer.append(data, len);

    parseBuffer();
}

void SensorDataParser::parseBuffer()
{
    if (_buffer.length() < 4) return; // 4  is the size of a complete data point

    char tag = _buffer.at(0);
    bool ok;
    float data = QString(_buffer.mid(1, 3)).toFloat(&ok);

    if (ok)
    {
        switch (tag)
        {
        case DATATAG_WHEELPOWER_A:
            _wheelPowerASeries.update(QVariant(data));
            break;
        case DATATAG_WHEELPOWER_B:
            _wheelPowerBSeries.update(QVariant(data));
            break;
        case DATATAG_WHEELPOWER_C:
            _wheelPowerCSeries.update(QVariant(data));
            break;
        case DATATAG_WHEELPOWER_D:
            _wheelPowerDSeries.update(QVariant(data));
            break;
        case DATATAG_WHEELPOWER_E:
            _wheelPowerESeries.update(QVariant(data));
            break;
        case DATATAG_WHEELPOWER_F:
            _wheelPowerFSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_REAR_YAW:
            _imuRearYawSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_REAR_PITCH:
            // P: [ 100 - 800 ] -> [ -90, 90 ]
            //data = (data - 100) * (180/800) - 90;
            _imuRearPitchSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_REAR_ROLL:
            // R: [ 100 - 900 ] -> [ -180, 180 ]
            //data = (data - 100) * (360/800) - 180;
            _imuRearRollSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_FRONT_YAW:
            _imuFrontYawSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_FRONT_PITCH:
            // P: [ 100 - 800 ] -> [ -90, 90 ]
            //data = (data - 100) * (180/800) - 90;
            _imuFrontPitchSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_FRONT_ROLL:
            // R: [ 100 - 900 ] -> [ -180, 180 ]
            //data = (data - 100) * (360/800) - 180;
            _imuFrontRollSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_MIDDLE_YAW:
            _imuMiddleYawSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_MIDDLE_PITCH:
            // P: [ 100 - 800 ] -> [ -90, 90 ]
            //data = (data - 100) * (180/800) - 90;
            _imuMiddlePitchSeries.update(QVariant(data));
            break;
        case DATATAG_IMUDATA_MIDDLE_ROLL:
            // R: [ 100 - 900 ] -> [ -180, 180 ]
            //data = (data - 100) * (360/800) - 180;
            _imuMiddleRollSeries.update(QVariant(data));
            break;
        default:
            // Something went wrong trying to parse
            // Remove the first char and try to parse again
            _buffer.remove(0, 1);
            parseBuffer();
            return;
        }
        Q_EMIT dataParsed(tag, data);
        _buffer.remove(0, 4);
        parseBuffer();
    }
    else
    {
        // Something went wrong trying to parse
        // Remove the first char and try to parse again
        _buffer.remove(0, 1);
        parseBuffer();
    }
}

const SensorDataParser::WheelPowerACsvSeries* SensorDataParser::getWheelPowerASeries() const
{
    return &_wheelPowerASeries;
}

const SensorDataParser::WheelPowerBCsvSeries* SensorDataParser::getWheelPowerBSeries() const
{
    return &_wheelPowerBSeries;
}

const SensorDataParser::WheelPowerCCsvSeries* SensorDataParser::getWheelPowerCSeries() const
{
    return &_wheelPowerCSeries;
}

const SensorDataParser::WheelPowerDCsvSeries* SensorDataParser::getWheelPowerDSeries() const
{
    return &_wheelPowerDSeries;
}

const SensorDataParser::WheelPowerECsvSeries* SensorDataParser::getWheelPowerESeries() const
{
    return &_wheelPowerESeries;
}

const SensorDataParser::WheelPowerFCsvSeries* SensorDataParser::getWheelPowerFSeries() const
{
    return &_wheelPowerFSeries;
}

const SensorDataParser::ImuRearYawCsvSeries* SensorDataParser::getImuRearYawSeries() const
{
    return &_imuRearYawSeries;
}

const SensorDataParser::ImuRearPitchCsvSeries* SensorDataParser::getImuRearPitchSeries() const
{
    return &_imuRearPitchSeries;
}

const SensorDataParser::ImuRearRollCsvSeries* SensorDataParser::getImuRearRollSeries() const
{
    return &_imuRearRollSeries;
}

const SensorDataParser::ImuFrontYawCsvSeries* SensorDataParser::getImuFrontYawSeries() const
{
    return &_imuFrontYawSeries;
}

const SensorDataParser::ImuFrontPitchCsvSeries* SensorDataParser::getImuFrontPitchSeries() const
{
    return &_imuFrontPitchSeries;
}

const SensorDataParser::ImuFrontRollCsvSeries* SensorDataParser::getImuFrontRollSeries() const
{
    return &_imuFrontRollSeries;
}

const SensorDataParser::ImuMiddleYawCsvSeries* SensorDataParser::getImuMiddleYawSeries() const
{
    return &_imuMiddleYawSeries;
}

const SensorDataParser::ImuMiddlePitchCsvSeries* SensorDataParser::getImuMiddlePitchSeries() const
{
    return &_imuMiddlePitchSeries;
}

const SensorDataParser::ImuMiddleRollCsvSeries* SensorDataParser::getImuMiddleRollSeries() const
{
    return &_imuMiddleRollSeries;
}

bool SensorDataParser::isValidTag(char c)
{
    return (c == DATATAG_WHEELPOWER_A) ||
            (c == DATATAG_WHEELPOWER_B) ||
            (c == DATATAG_WHEELPOWER_C) ||
            (c == DATATAG_WHEELPOWER_D) ||
            (c == DATATAG_WHEELPOWER_E) ||
            (c == DATATAG_WHEELPOWER_F) ||
            (c == DATATAG_IMUDATA_REAR_YAW) ||
            (c == DATATAG_IMUDATA_REAR_PITCH) ||
            (c == DATATAG_IMUDATA_REAR_ROLL) ||
            (c == DATATAG_IMUDATA_FRONT_YAW) ||
            (c == DATATAG_IMUDATA_FRONT_PITCH) ||
            (c == DATATAG_IMUDATA_FRONT_ROLL) ||
            (c == DATATAG_IMUDATA_MIDDLE_YAW) ||
            (c == DATATAG_IMUDATA_MIDDLE_PITCH) ||
            (c == DATATAG_IMUDATA_MIDDLE_ROLL);
}

} // namespace Soro
