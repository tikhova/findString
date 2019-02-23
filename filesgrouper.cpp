#include "filesgrouper.h"

#include <QTextStream>
#include <fstream>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QMessageBox>
#include <QtDebug>
#include <QElapsedTimer>
#include <QThread>

QMap<qint64, QFileInfoList> FilesGrouper::computeSizeGroups() {
    QMap<qint64, QFileInfoList> result;
    QDirIterator it(DIRECTORY, QDir::Hidden | QDir::Files | QDir::NoSymLinks |
                    QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        if(QThread::currentThread()->isInterruptionRequested()){
            qDebug()<<"computeSizeGroups: search interrupted";
            emit finished(QDialog::Accepted);
            return result;
        }
        QFileInfo  file = it.next();
        result[file.size()].push_back(file);
        ++count;
    }
    emit filesCounted(count);
    return result;
}

QByteArray FilesGrouper::getHash(QFileInfo const & fileInfo) {
    if(QThread::currentThread()->isInterruptionRequested()){
        qDebug()<<"getHash: search interrupted";
        emit finished(QDialog::Accepted);
        return nullptr;
    }
    QFile file(fileInfo.filePath());
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox msg(QMessageBox::Information, "Msg",
                        "Can't open " + file.fileName(), QMessageBox::Ok);
        return nullptr;
    }
    QCryptographicHash hash(QCryptographicHash::Algorithm::Sha256);
    hash.addData(&file);
    return hash.result();
}


QMap<QByteArray, QFileInfoList> FilesGrouper::computeHashGroups(QFileInfoList const & list) {
    QMap<QByteArray, QFileInfoList> result;
    for (QFileInfo file: list) {
        if(QThread::currentThread()->isInterruptionRequested()){
            qDebug()<<"computeHashGroups: search interrupted";
            emit finished(QDialog::Accepted);
            return result;
        }
        result[getHash(file)].push_back(file);
    }
    return result;
}

bool FilesGrouper::compareFiles(QFileInfo const & firstInfo, QFileInfo const & secondInfo) {
    std::ifstream first(firstInfo.path().toStdString(), std::ios::binary);
    std::ifstream second(secondInfo.path().toStdString(), std::ios::binary);
    std::string firstBuf, secondBuf;
    do {
        first.get(&firstBuf[0], 1024);
        second.get(&secondBuf[0], 1024);
        if (firstBuf != secondBuf) {
            return false;
        }
    } while(firstBuf.length() && secondBuf.length());
    return true;
}

QFileInfoList FilesGrouper::getFileGroup(QFileInfoList & list) {
    QFileInfoList changedList;
    QFileInfo file = list.front();
    QFileInfoList group;
    group.push_back(file);
    if (list.size() > 1) {
        for(QFileInfoList::const_iterator lit = list.cbegin() + 1; lit != list.cend(); ++lit) {
            if(QThread::currentThread()->isInterruptionRequested()){
                qDebug()<<"getFileGroup: search interrupted";
                emit finished(QDialog::Accepted);
                return group;
            }
            if (compareFiles(file, *lit)) {
                    group.push_back(*lit);
            } else {
                changedList.push_back(*lit);
            }
        }
    }
    list = changedList;
    checked += group.size();
    emit filesChecked(checked);
    return group;
}

std::list<QFileInfoList> FilesGrouper::computeDuplicateGroups(QMap<qint64, QFileInfoList> sizeList) {
    std::list<QFileInfoList> result;
    for (QMap<qint64, QFileInfoList>::iterator it = sizeList.begin(); it!= sizeList.end(); ++it) {
        if (it->size() > 1) {
            if(QThread::currentThread()->isInterruptionRequested()){
                qDebug()<<"computeDuplicateGroups: search interrupted";
                emit finished(QDialog::Accepted);
                return result;
            }
            QMap<QByteArray, QFileInfoList> hashGroups = computeHashGroups(*it);
            for (QMap<QByteArray, QFileInfoList>::iterator i = hashGroups.begin(); i != hashGroups.end(); ++i) {
                while (!i->isEmpty()) {
                    if(QThread::currentThread()->isInterruptionRequested()){
                        qDebug()<<"computeDuplicateGroups: search interrupted";
                        emit finished(QDialog::Accepted);
                        return result;
                    }
                    QFileInfoList res = getFileGroup(*i);
                    if (res.size() > 1) {
                        result.push_back(res);
                    } else if (res.size() == 0) break;
                }
            }
        }
    }
    return result;
}

void FilesGrouper::computeDuplicateGroups() {
    QElapsedTimer timer;
    timer.start();
    groups = computeDuplicateGroups(computeSizeGroups());
    qDebug()<<"Computation time: " << timer.elapsed() << "ms.";
    emit finished(QDialog::Accepted);
}
