#ifndef FILESGROUPER_H
#define FILESGROUPER_H

#include <QString>
#include <QDir>

class FilesGrouper : public QObject {
Q_OBJECT
public:
    FilesGrouper(QString const &dir) : DIRECTORY(dir) {}
public slots:
    std::list<QFileInfoList> getDuplicateGroups() { return groups; }
    void computeDuplicateGroups();
private:
    int count = 0;
    int checked = 0;
    QString DIRECTORY;
    qint64 const CHUNK_SIZE = 1024;
    std::list<QFileInfoList> groups;
    QByteArray getHash(QFileInfo const &);
    bool compareFiles(QFileInfo const &, QFileInfo const &);
    QFileInfoList getFileGroup(QFileInfoList &);
    std::list<QFileInfoList> computeDuplicateGroups(QMap<qint64, QFileInfoList>);
    QMap<qint64, QFileInfoList> computeSizeGroups();
    QMap<QByteArray, QFileInfoList> computeHashGroups(QFileInfoList const &);

signals:
    void filesCounted(int);
    void filesChecked(int);
    void finished(int);
};

#endif // FILESGROUPER_H
