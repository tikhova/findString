#include "finder.h"

#include <QDirIterator>
#include <QSet>
#include <QFileInfo>
#include <fstream>
#include <QDebug>
#include <QDialog>
#include <QThread>

void finder::findString() {
    qDebug() << "find string" << string;
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
            resultMap = result;
            return;
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
    resultMap = result;
    qDebug() << "find string finished"  << string;
    return;
}
