#ifndef INDEXATOR_H
#define INDEXATOR_H

#include "trigram.h"

#include <QString>
#include <QDir>
#include <QMap>
#include <set>
#include <QFileSystemWatcher>

#include <bits/unique_ptr.h>

class indexator : public QObject {
Q_OBJECT
public:
    bool indexationFinished = false;
    indexator() : fsw(new QFileSystemWatcher) { }
public slots:
    void getTrigrams();
    void setDirectory(QString const &dir) {
        DIRECTORY = dir;connect(fsw.get(), SIGNAL(directoryChanged(QString)), this, SLOT(getTrigrams()));
        connect(fsw.get(), SIGNAL(fileChanged(QString)), this, SLOT(getTrigrams(QString)));
    }
    QMap<QString, QStringList> findString(QString const &);
    void getTrigrams(QString const &);
private:
    int count = 0;
    int checked = 0;
    QString DIRECTORY;
    size_t const CHUNK_SIZE = 1024 * 64;
    std::unique_ptr<QFileSystemWatcher> fsw;
    QMap<QString, std::set<trigram>> trigramsMap;

signals:
    void filesCounted(int);
    void filesChecked(int);
    void finished(int);
};


#endif // INDEXATOR_H
