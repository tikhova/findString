#ifndef FILESGROUPER_H
#define FILESGROUPER_H

#include <QString>
#include <QFile>
#include <QDir>

QFileInfoList getFilesList(QString dirPath);

std::map<long long, QFileInfoList> getSizeGroups(const QFileInfoList & list);
std::list<QFileInfoList> getFileGroups(std::map<long long, QFileInfoList> sizeList);

void printOutSizeGroups(const std::map<long long, QFileInfoList> & sizeGroupsMap);
void printOutFileGroups(const std::list<QFileInfoList> & sizeGroupsMap);


#endif // FILESGROUPER_H
