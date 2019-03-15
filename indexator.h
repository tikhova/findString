#ifndef INDEXATOR_H
#define INDEXATOR_H

#include "trigram.h"

#include <QString>
#include <QDir>
#include <QMap>
#include <set>
#include <QFileSystemWatcher>
#include <QMutex>

#include <bits/unique_ptr.h>

class indexator : public QObject {
Q_OBJECT
public:
    bool indexationFinished = false;
    indexator() : fsw(new QFileSystemWatcher) {}
//    inline QMap<QString, QStringList> getResultMap() { return resultMap; }
    inline QMap<QString, std::set<trigram>> const & getTrigramsMap() {return trigramsMap;}
    void setDirectory(QString const &dir) {
        DIRECTORY = dir;
        connect(fsw.get(), SIGNAL(directoryChanged(QString)), this, SLOT(getTrigrams()));
        connect(fsw.get(), SIGNAL(fileChanged(QString)), this, SLOT(getTrigrams(QString)));
    }/*
    inline void setString(const QString & str) {string = str;}*/
    QMutex mutex;
public slots:
    void getTrigrams();
//    void findString();
    void getTrigrams(QString const &);
private:
    QString DIRECTORY;
    size_t const CHUNK_SIZE = 1024 * 64;
    std::unique_ptr<QFileSystemWatcher> fsw;
    QMap<QString, std::set<trigram>> trigramsMap;
//    QMap<QString, QStringList> resultMap;
//    QString string;

signals:
    void filesCounted(int);
    void filesChecked(int);
    void finished(int);
};


#endif // INDEXATOR_H
