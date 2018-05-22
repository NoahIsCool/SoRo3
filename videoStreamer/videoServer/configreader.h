#ifndef CONFIGREADER_H
#define CONFIGREADER_H

#include <QObject>
#include <vector>
#include <QFile>
#include <QTextStream>

#include "logger.h"

struct StringPair{
    QString tag;
    QString value;
};

class ConfigReader : public QObject
{
    Q_OBJECT
public:
    explicit ConfigReader(QObject *parent = nullptr);
    ConfigReader(QString configFile);
    QString find(QString tag);
    bool exists();

signals:

public slots:

private:
    std::vector<StringPair> list;
    bool doesExist = true;
    const QString LOG_TAG = "Config Reader";
};

#endif // CONFIGREADER_H
