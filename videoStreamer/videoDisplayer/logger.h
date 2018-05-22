#ifndef LOGGER_H
#define LOGGER_H

#include <QString>
#include <iostream>

namespace Logger{

void LOG_I(QString logTag,QString message,bool save=false);
void LOG_W(QString logTag,QString message,bool save=false);
void LOG_E(QString logTag,QString message,bool save=false);

}

#endif // LOGGER_H
