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
    QFileInfoList filesList = QDir(dirPath).entryInfoList();
    QFileInfoList dirList;
    //const int listSize = filesList.size();
    for (int i = 0; i != filesList.size(); ++i) {
        if (filesList.at(i).isDir()) {
            dirList.push_back(filesList.at(i));
            filesList.removeAt(i);
        }
    }
    const int dirListSize = dirList.size();
    for (int i = 0; i != dirListSize; ++i) {
        filesList.append(getFilesList(dirList.at(i).path()));
    }
    return filesList;
}

std::map<long long, QFileInfoList> getSizeGroups(const QFileInfoList & list) {
    std::map<long long, QFileInfoList> result;
    const int listSize = list.size();
    long long size = 0;
    for (int i = 0; i != listSize; ++i) {
        size = list.at(i).size();
        if (result.find(size) == result.end()) {
            result.insert(std::pair<long long, QFileInfoList>(size, QFileInfoList()));
        }
        result.find(size)->second.push_back(list.at(i));
    }
    return result;
}

void printOutSizeGroups(const std::map<long long, QFileInfoList> & sizeGroupsMap) {
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

void printOutFileGroups(const std::list<QFileInfoList> & sizeGroupsMap) {
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
    std::ifstream file(fileInfo.path().toStdString(), std::ios::binary);
    long long hash = 0;
    std::string buffer;
    std::hash<std::string> hash_fn;
    while(!file.eof()) {
        file.get(&buffer[0], 1024);
        hash += hash_fn(buffer);
    }
    return hash;
}

bool compareFiles(QFileInfo const & firstInfo, QFileInfo const & secondInfo) {
    std::ifstream first(firstInfo.path().toStdString(), std::ios::binary);
    std::ifstream second(secondInfo.path().toStdString(), std::ios::binary);
    std::string firstBuf, secondBuf;
    while(!first.eof() && !second.eof()) {
        first.get(&firstBuf[0], 1024);
        second.get(&secondBuf[0], 1024);
        if (firstBuf != secondBuf) {
            return false;
        }
    }
    return true;
}

QFileInfoList getFileGroup(QFileInfoList & list, QFileInfo const & file) {
    QFileInfoList resulting_list;
    long long mainHash = getHash(file);
    QFileInfoList result;
    result.push_back(file);
    for(QFileInfoList::const_iterator lit = list.cbegin(); lit != list.cend(); ++lit) {
        if (mainHash == getHash(*lit)) {
            if (compareFiles(file, *lit)) {
                result.push_back(*lit);
            } else {
                resulting_list.push_back(*lit);
            }
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
            result.push_back(getFileGroup(it->second, it->second.front()));
        }
    }
    return result;
}
