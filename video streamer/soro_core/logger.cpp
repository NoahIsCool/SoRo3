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

#include "logger.h"

namespace Soro {

Logger* Logger::_root = new Logger();

Logger::Logger(QObject *parent) : QObject(parent)
{
    // create default text formatting
    _stdoutFormat << "\033[31m[E]\033[0m %1 \033[35m%2\033[0m: %3";
    _stdoutFormat << "\033[33m[W]\033[0m %1 \033[35m%2\033[0m: %3";
    _stdoutFormat << "\033[34m[I]\033[0m %1 \033[35m%2\033[0m: %3";
    _stdoutFormat << "[D] %1 \033[35m%2\033[0m: %3";

    // default unless later set otherwise
    _textFormat << "[E]\t%1\t%2:\t%3";
    _textFormat << "[W]\t%1\t%2:\t%3";
    _textFormat << "[I]\t%1\t%2:\t%3";
    _textFormat << "[D]\t%1\t%2:\t%3";
}

bool Logger::setLogfile(QString file)
{
    _fileMutex.lock();
    if (_fileStream != nullptr)
    {
        _fileStream->flush();
        delete _fileStream;
        _fileStream = nullptr;
    }
    if (_file != nullptr)
    {
        if (_file->isOpen()) _file->close();
        delete _file;
    }
    _file = new QFile(file, this);
    if (_file->open(QIODevice::Append))
    {
        _fileStream = new QTextStream(_file);
        _fileMutex.unlock();
        return true;
    }
    _fileMutex.unlock();
    // could not open the file
    e("LOGGER", "Unable to open the specified logfile for write access (" + file + ")");
    return false;
}

void Logger::publish(Level level, QString tag, QString message)
{
    // check for file output
    if (level <= _maxFileLevel)
    {
        QString formatted = _textFormat[reinterpret_cast<int&>(level) - 1]
                .arg(QTime::currentTime().toString(), tag, message);
        _fileMutex.lock();
        if (_fileStream != nullptr)
        {
            *_fileStream << formatted << endl;
            _fileStream->flush();
        }
        _fileMutex.unlock();
    }
    // check for Qt logger output
    if (level <= _maxQtLogLevel)
    {
        QString formatted = _stdoutFormat[reinterpret_cast<int&>(level) - 1]
                .arg(QTime::currentTime().toString(), tag, message);
        QTextStream(stdout) << formatted << endl;
    }
}

void Logger::closeLogfile()
{
    _fileMutex.lock();
    if (_file)
    {
        if (_fileStream)
        {
            delete _fileStream;
            _fileStream = nullptr;
        }
        if (_file->isOpen())
        {
            _file->close();
        }
        delete _file;
        _file = nullptr;
    }
    _fileMutex.unlock();
}

void Logger::setMaxFileLevel(Logger::Level maxLevel)
{
    _maxFileLevel = maxLevel;
}

void Logger::setMaxStdoutLevel(Logger::Level maxLevel)
{
    _maxQtLogLevel = maxLevel;
}

void Logger::setOutputFileTextFormat(const QStringList& format)
{
    if (format.size() != 4)
    {
        e("LOGGER", "Attempted to set invalid text formatting");
    }
    else {
        _textFormat = format;
    }
}

Logger::~Logger()
{
    closeLogfile();
}

}
