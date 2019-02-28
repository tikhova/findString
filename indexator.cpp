#include "indexator.h"

#include <QDirIterator>
#include <QSet>
#include <QFileInfo>
#include <fstream>

void indexator::getTrigrams() {
    QDirIterator it(DIRECTORY, QDir::Hidden | QDir::Files | QDir::NoSymLinks |
                    QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QFileInfoList files;
    while (it.hasNext()) {
        QFileInfo  file = it.next();
        if (file.isFile() && file.isReadable()) {
            files.push_back(file);
        }
        ++count;
    }


    for(auto & file: files) {
        QSet<trigram> trigrams = getTrigrams(file);
        if (trigrams.size())
            trigramsMap.insert(file.filePath(), trigrams);
    }
}

QSet<trigram> indexator::getTrigrams(QFileInfo const & file) {
    std::ifstream fin(file.fileName().toStdString(), std::ios::in | std::ios::binary);
    std::vector<char> buffer(CHUNK_SIZE);
    QSet<trigram> result;
    char previous[2] = {0, 0};
    int count;
    while (!fin.eof()) {
        fin.read(buffer.data(), CHUNK_SIZE);
        count = fin.gcount();
        for (int i = 0; i != count; ++i) {
            if (i >= 2)
                result.insert(trigram(buffer[i - 2], buffer[i - 1], buffer[i]));
            else if (previous[0] != 0) {
                result.insert(trigram(previous[0], previous[1], buffer[i]));
                previous[0] = previous[1];
                previous[1] = buffer[i];
            }
        }

        previous[0] = buffer[count - 2];
        previous[1] = buffer[count - 1];
    }
    return result;
}
