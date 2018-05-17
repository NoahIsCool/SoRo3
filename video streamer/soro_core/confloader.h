#ifndef SORO_CONFLOADER_H
#define SORO_CONFLOADER_H

#include <QtCore>
#include <QHostAddress>

#include "soro_core_global.h"

namespace Soro {

/* A simple class to parse a tag-value formatted configuration file (INI/CONF format)
 * from a QTextStream
 *
 * - Call load(QTextStream) to read in a file
 * - Call value(QString&), valueAsInt(QString&), etc to get a tag's value
 */
class SORO_CORE_EXPORT ConfLoader {

private:
     QMap<QString, QString> _contents;

public:
     /* Loads a configuration file from a QTextStream.
     *
     * QTextStreams can be created from any QIODevice, such as
     * QFile and QNetworkResponse.
     *
     * Returns true if the file was read in successfully, false otherwise.
     *
     * As soon as this completes, it is safe to close the original file/network
     * resources as everything will be loaded locally.
     */
    bool load(QTextStream& fileStream);

    /* Loads a configuration file from a file.
     *
     * Returns true if the file was read in successfully, false otherwise.
     */
    bool load(QFile& file);

    bool write(QFile& file) const;

    /* Gets a tag's value from the last file read in.
     */
    QString value(const QString& tag) const;

    /* Gets a tag's value from the last file read in as an integer,
     * and returns true if the conversion was successful.
     */
    bool valueAsInt(const QString& tag, int *value) const;

    /* Gets a tag's value from the last file read in as a range of integers
     * and returns true if the conversion was successful
     */
    bool valueAsIntRange(const QString& tag, int *values) const;

    /* Gets a tag's value from the last file read in as an boolean,
     * and returns true if the conversion was successful.
     */
    bool valueAsBool(const QString& tag, bool *value) const;

    /* Gets a tag's value from the last file read in as an QHostAddress,
     * and returns true if the conversion was successful.
     */
    bool valueAsIP(const QString& tag, QHostAddress *value, bool allowV6) const;

    /* Gets a tag's value from the last file read in as a list of strings,
     * separated by commas.
     */
    QList<QString> valueAsStringList(const QString& tag) const;

    /* Gets the list of tags from the last file read in.
     */
    QList<QString> tags() const;

    /* Returns true if the tag was contained in the last file read in, false otherwise.
     */
    bool contains(const QString& tag) const;

    /* Gets the number of tags/value pairs read in.
     */
    int count() const;

    /* Removes a loaded tag/value pair. Does not modify the original file.
     */
    bool remove(const QString& tag);

    /* Inserts a tag/value pair
     */
    void insert(const QString& tag, const QString& value);
};

}

#endif // SORO_CONFLOADER_H
