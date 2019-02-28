#ifndef INDEXATOR_H
#define INDEXATOR_H

#include "trigram.h"

#include <QString>
#include <QDir>
#include <QSet>
#include <QMap>

class indexator : public QObject {
Q_OBJECT
public:
    indexator(QString const &dir) : DIRECTORY(dir) {}
public slots:

private:
    int count = 0;
    int checked = 0;
    QString DIRECTORY;
    qint64 const CHUNK_SIZE = 1024;
    QMap<QString, QSet<trigram>> trigramsMap;
    void getTrigrams();
    QSet<trigram> getTrigrams(QFileInfo const &);

signals:
    void filesCounted(int);
    void filesChecked(int);
    void finished(int);
};


#endif // INDEXATOR_H
