#ifndef FINDER_H
#define FINDER_H
#include "indexator.h"
#include "trigram.h"
#include <QString>
#include <QDir>
#include <QMap>
#include <set>
#include <QFileSystemWatcher>
#include <QMutex>
#include <bits/unique_ptr.h>

class finder: public QObject {
    Q_OBJECT
public:
    finder(indexator& ind) : mutex(ind.mutex), trigramsMap(ind.getTrigramsMap()) {}
    inline QMap<QString, QStringList> getResultMap() { return resultMap; }
public slots:
    void findString();
    inline void setString(const QString & str) {string = str;}
private:
    QMap<QString, std::set<trigram>> const & trigramsMap;
    QMap<QString, QStringList> resultMap;
    QString string;
    QMutex& mutex;
signals:
    void filesCounted(int);
    void filesChecked(int);
    void finished(int);
};

#endif // FINDER_H
