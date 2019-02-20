#include "filesgrouper.h"

#include <QTextStream>
#include <unordered_map>
#include <QDir>
#include <fstream>
#include <bits/stl_pair.h>
#include <iostream>
#include <QCryptographicHash>
#include <QDebug>
#include <utility>
#include <string>
#include <functional>

bool isExistingDir(QString path) {
    QFileInfo file(path);
    return file.exists() && file.isDir();
}

QFileInfoList getFilesList(QString dirPath) {
    QDir dir = QDir(dirPath);
    QFileInfoList filesList = dir.entryInfoList();
    QStringList dirList;
    for (int i = 0; i != filesList.size(); ++i) {
        if (filesList.at(i).isDir()) {
            if (filesList.at(i).canonicalFilePath().length() > dirPath.length()) {
                dirList.push_back(filesList.at(i).filePath());
            }
            filesList.removeAt(i);
            --i;
        }
    }
    const int dirListSize = dirList.size();
    for (int i = 0; i != dirListSize; ++i) {
        filesList.append(getFilesList(dirList.at(i)));
    }
    return filesList;
}

std::map<long long, QFileInfoList> getSizeGroups(const QFileInfoList & list) {
    std::map<long long, QFileInfoList> result;
    const int listSize = list.size();
    long long size = 0;
    for (int i = 0; i != listSize; ++i) {
        size = list.at(i).size();
        if (result.find(size) == result.end())
            result.insert(std::pair<long long, QFileInfoList>(size, QFileInfoList()));
        result.find(size)->second.push_back(list.at(i));
    }
    return result;
}

void printOutSizeGroups(std::map<long long, QFileInfoList> const & sizeGroupsMap) {
    QString filename="result.txt";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) ) {
        QTextStream stream( &file );
        for (std::map<long long, QFileInfoList>::const_iterator it = sizeGroupsMap.cbegin();
             it != sizeGroupsMap.cend(); ++it) {
            stream << it->first << ": " << endl;
            for (QFileInfoList::const_iterator lit = it->second.cbegin(); lit != it->second.cend(); ++lit) {
                stream << lit->filePath() << endl;
            }
            stream << endl;
        }
    }
}

void printOutFileGroups( std::list<QFileInfoList> const & sizeGroupsMap) {
    QString filename="result.txt";
    QFile file( filename );
    if ( file.open(QIODevice::ReadWrite) ) {
        QTextStream stream( &file );
        for (std::list<QFileInfoList>::const_iterator it = sizeGroupsMap.cbegin();
             it != sizeGroupsMap.cend(); ++it) {
            for (QFileInfoList::const_iterator lit = it->cbegin(); lit != it->cend(); ++lit) {
                stream << lit->filePath() << endl;
            }
            stream << endl;
        }
    }
}

long long getHash(QFileInfo const & fileInfo) {
QFile file(fileInfo.path());

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray fileData = file.readAll();

        QByteArray hashData = QCryptographicHash::hash(fileData,QCryptographicHash::Sha256);
        return hashData.toLongLong();
    }
    return 0;
}

bool compareFiles(QFileInfo const & firstInfo, QFileInfo const & secondInfo) {
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

QFileInfoList getFileGroup(QFileInfoList & list) {
    QFileInfoList resulting_list;
    QFileInfo file = list.front();
    long long mainHash = getHash(file);
    QFileInfoList result;
    for(QFileInfoList::const_iterator lit = list.cbegin(); lit != list.cend(); ++lit) {
        if (mainHash == getHash(*lit) && compareFiles(file, *lit)) {
                result.push_back(*lit);
        } else {
            resulting_list.push_back(*lit);
        }
    }
    list = resulting_list;
    return result;
}

std::list<QFileInfoList> getFileGroups(std::map<long long, QFileInfoList> sizeList) {
    std::list<QFileInfoList> result;
    for (std::map<long long, QFileInfoList>::iterator it = sizeList.begin(); it!= sizeList.end(); ++it) {
        while(!it->second.isEmpty()) {
            result.push_back(getFileGroup(it->second));
        }
    }
    return result;
}
