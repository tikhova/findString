#include "indexator.h"

#include <QDirIterator>
#include <QSet>
#include <QFileInfo>
#include <fstream>
#include <QDebug>
#include <QDialog>
#include <QThread>

void indexator::getTrigrams() {
    emit filesChecked(0);
    trigramsMap.clear();
    indexationFinished = false;
    qDebug() << "start getTrigrams";
    QDirIterator it(DIRECTORY, QDir::Hidden | QDir::Files | QDir::NoSymLinks |
                    QDir::NoDotAndDotDot | QDir::Readable, QDirIterator::Subdirectories);
    QFileInfoList files;
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()){
            qDebug()<<"getTrigrams: search interrupted";
            emit finished(QDialog::Accepted);
            return;
        }
        QFileInfo  file = it.next();
        if (file.isFile() && file.isReadable()) {
            files.push_back(file);
        }
    }
    emit filesCounted(files.size());

    qDebug() << "files list computated";

    int checked = 0;
    for(auto & file: files) {
        if (QThread::currentThread()->isInterruptionRequested()){
            qDebug()<<"getTrigrams: search interrupted";
            emit finished(QDialog::Accepted);
            return;
        }
        getTrigrams(file.filePath());
        ++checked;
        emit filesChecked(checked);
    }
    qDebug() << "finish getTrigrams";
    indexationFinished = true;
}

void indexator::getTrigrams(QString const & file) {
    static std::set<char> non_valid_char = {0, 8, 24, 0x7F};
    std::ifstream fin(file.toStdString(), std::ios::binary);
    std::vector<char> buffer(CHUNK_SIZE);
    std::set<trigram> result;
    trigramsMap.erase(trigramsMap.find(file));
    char previous[2] = {0, 0};
    size_t count;
    while (fin.good()) {
        fin.read(buffer.data(), static_cast<std::streamsize>(CHUNK_SIZE));
        count = static_cast<size_t>(fin.gcount());
        for (size_t i = 0; i != count; ++i) {
            if (i >= 2)
                result.insert(trigram(buffer[i - 2], buffer[i - 1], buffer[i]));
            else if (previous[0] != 0) {
                result.insert(trigram(previous[0], previous[1], buffer[i]));
                previous[0] = previous[1];
                previous[1] = buffer[i];
            }

            if (non_valid_char.find(buffer[i]) != non_valid_char.end()) return;
        }

        previous[0] = buffer[count - 2];
        previous[1] = buffer[count - 1];

        if (result.size() > 200000) return;
    }
    trigramsMap.insert(file, result);
}


QMap<QString, QStringList> indexator::findString(QString const & string) {
    QMutexLocker locker(&mutex);
    emit filesChecked(0);
    emit filesCounted(trigramsMap.size());
    std::string str = string.toStdString();
    QMap<QString, QStringList> result;
    std::set<trigram> str_trigrams;
    size_t count = str.length();
    // Compute string trigrams
    for (size_t i = 2; i < count; ++i)
        str_trigrams.insert(trigram(str[i - 2], str[i - 1], str[i]));

    int checked = 0;
    QMapIterator<QString, std::set<trigram>> iter(trigramsMap);
    while(iter.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()){
            qDebug()<<"findString: search interrupted";
            emit finished(QDialog::Accepted);
            return result;
        }
        iter.next();
        // Compare trigrams sets
        if(std::includes(iter.value().begin(), iter.value().end(),
        str_trigrams.begin(), str_trigrams.end())) {
            QFile file(iter.key());
            if (file.open(QIODevice::ReadOnly)) {
                // Try to find string in file
                while (!file.atEnd()) {
                    QByteArray line = file.readLine();
                    if (line.contains(string.toUtf8())) {
                        result.insert(iter.key(), QStringList());
                        break;
                    }
                }
            }
        }
        ++checked;
        emit filesChecked(checked);
    }
    emit finished(QDialog::Accepted);
    return result;
}
