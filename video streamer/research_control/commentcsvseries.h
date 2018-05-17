#ifndef COMMENTCSVSERIES_H
#define COMMENTCSVSERIES_H

#include "soro_core/csvrecorder.h"

namespace Soro {

class CommentCsvSeries : public CsvDataSeries
{
    Q_OBJECT
public:
    CommentCsvSeries(QObject *parent=0);
    QString getSeriesName() const;
    bool shouldKeepOldValues() const;

public Q_SLOTS:
    void onCommentEntered(QString comment);
};

} // namespace Soro

#endif // COMMENTCSVSERIES_H
