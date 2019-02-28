#ifndef TRIGRAM_H
#define TRIGRAM_H

#include <limits>
#include <QHash>

struct trigram {
    char symbols[3];
    trigram(char const & c0, char const & c1, char const & c2) {
        symbols[0] = c0;
        symbols[1] = c1;
        symbols[2] = c2;
    }
};

#endif // TRIGRAM_H
