#ifndef FILESGROUPER_H
#define FILESGROUPER_H

#include <QString>
#include <QFile>
#include <QDir>

QFileInfoList getFilesList(QString dirPath);

std::map<long long, QFileInfoList> getSizeGroups(QFileInfoList const & list);
std::list<QFileInfoList> getFileGroups(std::map<long long, QFileInfoList> sizeList);

void printOutSizeGroups(std::map<long long, QFileInfoList> const & sizeGroupsMap);
void printOutFileGroups(std::list<QFileInfoList> const & sizeGroupsMap);


#endif // FILESGROUPER_H
