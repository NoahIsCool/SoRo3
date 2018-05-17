#ifndef CSVRECORDER_H
#define CSVRECORDER_H

#include <QDateTime>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QTimerEvent>

#include "soro_core_global.h"
#include "constants.h"

namespace Soro {

/* Abstract class to represent a data series.
 * Accepts QVariants for data
 */
class SORO_CORE_EXPORT CsvDataSeries : public QObject
{
    Q_OBJECT

public:
    CsvDataSeries(QObject *parent=0);

    /* Gets the current value
     */
    QVariant getValue() const;

    /* Gets the timestamp for the current value.
     * Will be zero if no value has yet be entered.
     */
    qint64 getValueTime() const;
    /* The human-readable name for the data series.
     */
    virtual QString getSeriesName() const=0;
    /* If true, when a new row is addd to the CSV file, then the last available value
     * for this data serises should be duplicated. If false, a blank cell should be
     * output instead.
     */
    virtual bool shouldKeepOldValues() const=0;

protected:
    /* Called by the child class to update the current data
     */
    void update(QVariant value);

Q_SIGNALS:
    /* Should be emitted when the value changes.
     * This is only respected if the CsvRecorder using this series
     * is configured to log on demand.
     */
    void valueUpdated();

private:
    QVariant _value;
    qint64 _valueTime = 0;
};

/* Class to record data at a regular interval in a CSV formatted file
 */
class SORO_CORE_EXPORT CsvRecorder : public QObject
{
    Q_OBJECT
public:
    enum RecordingMode
    {
        /* A row should be inserted at a regular interval
         */
        RECORDING_MODE_ON_INTERVAL,
        /* A row should only be inserted when new data is available
         */
        RECORDING_MODE_ON_DEMAND,
    };

    CsvRecorder(QString logName, QObject *parent=0);

    bool isRecording() const;
    qint64 getStartTime() const;

    void addColumn(const CsvDataSeries* series);
    void removeColumn(const CsvDataSeries* series);
    void clearColumns();
    /* Set the update interval to use if the recording mode is RECORDING_MODE_ON_INTERVAL
     */
    void setUpdateInterval(int interval);
    int getUpdateInterval() const;
    const QList<const CsvDataSeries*>& getColumns() const;

public Q_SLOTS:
    /* Starts logging data in the specified file, and calculates all timestamps offset from
     * the provided start time.
     */
    bool startLog(QDateTime loggedStartTime, CsvRecorder::RecordingMode mode);

    /* Stops logging, if it is currently active.
     */
    void stopLog();

Q_SIGNALS:
    void logStarted(QDateTime loggedStartTime);
    void logStopped();

protected:
    void timerEvent(QTimerEvent *e);
    void logRow();

protected Q_SLOTS:
    void onSeriesUpdated();

private:
    QList<const CsvDataSeries*> _columns;
    QHash<const CsvDataSeries*, qint64> _columnDataTimestamps;
    int _updateTimerId = TIMER_INACTIVE;
    QTextStream *_fileStream = nullptr;
    QString _logDir;
    QString _logName;
    int _updateInterval;
    QFile *_file = nullptr;
    qint64 _logStartTime;
    bool _isRecording=false;
    RecordingMode _mode=RECORDING_MODE_ON_INTERVAL;
};

} // namespace Soro

#endif // CSVRECORDER_H
