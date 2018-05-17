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

#include "csvrecorder.h"
#include "logger.h"

#include <QCoreApplication>
#include <QDir>

#define LOG_TAG "CsvRecorder"

namespace Soro {

CsvDataSeries::CsvDataSeries(QObject *parent) : QObject(parent) { }

QVariant CsvDataSeries::getValue() const {
    return _value;
}

qint64 CsvDataSeries::getValueTime() const {
    return _valueTime;
}

void CsvDataSeries::update(QVariant value) {
    _valueTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (_value != value) {
        _value = value;
        Q_EMIT valueUpdated();
    }
}

////////////////////////////////////////

CsvRecorder::CsvRecorder(QString logName, QObject *parent) : QObject(parent)
{
    _updateInterval = 100;
    _logName = logName;
}

bool CsvRecorder::startLog(QDateTime loggedStartTime, CsvRecorder::RecordingMode mode)
{
    stopLog();

    QString filePath = QCoreApplication::applicationDirPath() + "/../research_data";

     if (!QDir(filePath).exists())
     {
        LOG_I(LOG_TAG, filePath + " directory does not exist, creating it");
        if (!QDir().mkpath(filePath))
        {
            LOG_E(LOG_TAG, "Cannot create " + filePath + " directory, data cannot be logged");
            return false;
        }
    }

    _logStartTime = loggedStartTime.toMSecsSinceEpoch();
    filePath += "/" + _logName + "_" + loggedStartTime.toString("M-dd_h.mm.ss_AP") + ".csv";
    _file = new QFile(filePath, this);

    if (_file->exists())
    {
        LOG_W(LOG_TAG, "File \'" + filePath + "\' already exists, overwriting it");
    }
    if (_file->open(QIODevice::WriteOnly))
    {
        _fileStream = new QTextStream(_file);
        // Write header to file
        *_fileStream << "Recording started at " << loggedStartTime.toString() << "\n";
        if (mode == RECORDING_MODE_ON_INTERVAL)
        {
            *_fileStream << "Rows in this file were updated every " << _updateInterval << " milleseconds\n";
        }
        else
        {
            *_fileStream << "Rows in this file were updated on demand\n";
        }
        *_fileStream << "\n";

        for (const CsvDataSeries* column : _columns)
        {
            *_fileStream << column->getSeriesName() << "," << column->getSeriesName() << " (timestamp),";
        }
        *_fileStream << "\n";

        LOG_I(LOG_TAG, "Starting log " + QString::number(_logStartTime));
        if (mode == RECORDING_MODE_ON_INTERVAL)
        {
            START_TIMER(_updateTimerId, _updateInterval);
        }
        else
        {
            for (const CsvDataSeries* column : _columns)
            {
               connect(column, &CsvDataSeries::valueUpdated, this, &CsvRecorder::onSeriesUpdated);
            }
        }
        _isRecording = true;
        Q_EMIT logStarted(loggedStartTime);
        return true;
    }
    // could not open the file
    LOG_E(LOG_TAG, "Unable to open the specified logfile for write access (" + filePath + ")");
    delete _file;
    _file = nullptr;
    _logStartTime = 0;
    return false;
}

void CsvRecorder::stopLog()
{
    if (_isRecording)
    {
        KILL_TIMER(_updateTimerId);
        for (const CsvDataSeries* column : _columns)
        {
           disconnect(column, &CsvDataSeries::valueUpdated, this, &CsvRecorder::onSeriesUpdated);
        }
        delete _fileStream;
        _fileStream = nullptr;
        LOG_I(LOG_TAG, "Ending log " + QString::number(_logStartTime));

        if (_file->isOpen())
        {
            _file->close();
        }
        delete _file;
        _file = nullptr;
        _isRecording = false;
        _logStartTime = 0;
        Q_EMIT logStopped();
    }
}

int CsvRecorder::getUpdateInterval() const
{
    return _updateInterval;
}

const QList<const CsvDataSeries*>& CsvRecorder::getColumns() const
{
    return _columns;
}

void CsvRecorder::setUpdateInterval(int interval)
{
    if (_isRecording)
    {
        LOG_E(LOG_TAG, "Cannot change update interval while recording");
        return;
    }
    _updateInterval = interval;
}

void CsvRecorder::addColumn(const CsvDataSeries *series)
{
    if (_isRecording)
    {
        LOG_E(LOG_TAG, "Cannot modify column array while recording");
        return;
    }
    if (!_columns.contains(series))
    {
        _columns.append(series);
        _columnDataTimestamps.insert(series, -1);
    }
}

void CsvRecorder::removeColumn(const CsvDataSeries *series)
{
    if (_isRecording)
    {
        LOG_E(LOG_TAG, "Cannot modify column array while recording");
        return;
    }
    if (_columns.contains(series))
    {
        _columns.removeAll(series);
        _columnDataTimestamps.remove(series);
    }
}

void CsvRecorder::onSeriesUpdated()
{
    logRow();
}

void CsvRecorder::clearColumns()
{
    if (_isRecording)
    {
        LOG_E(LOG_TAG, "Cannot modify column array while recording");
        return;
    }
    _columns.clear();
    _columnDataTimestamps.clear();
}

void CsvRecorder::logRow()
{
    if (_fileStream)
    {
        for (const CsvDataSeries *column : _columns)
        {
            if ((_columnDataTimestamps.value(column) != column->getValueTime()) || column->shouldKeepOldValues())
            {
                *_fileStream << column->getValue().toString() << "," << (column->getValueTime() > 0 ? QString::number(column->getValueTime() - _logStartTime) : "---") << ",";
                _columnDataTimestamps.insert(column, column->getValueTime());
            }
            else {
                *_fileStream << ",,";
            }
        }
        *_fileStream << "\n";
    }
}

void CsvRecorder::timerEvent(QTimerEvent *e)
{
    QObject::timerEvent(e);

    if ((e->timerId() == _updateTimerId))
    {
        logRow();
    }
}

bool CsvRecorder::isRecording() const {
    return _isRecording;
}

qint64 CsvRecorder::getStartTime() const {
    return _logStartTime;
}

} // namespace Soro
