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

inline uint qHash(const trigram &t) {
    return ((static_cast<uint>(t.symbols[0]) << (std::numeric_limits<char>::digits << 1)) ^
            (static_cast<uint>(t.symbols[1]) << std::numeric_limits<char>::digits) ^
             static_cast<uint>(t.symbols[2]));
}

inline bool operator==(const trigram& lhs, const trigram& rhs) {
    return (lhs.symbols[0] == rhs.symbols[0] &&
            lhs.symbols[1] == rhs.symbols[1] &&
            lhs.symbols[2] == rhs.symbols[2]);
}

inline bool operator<(const trigram& lhs, const trigram& rhs) {
    return (qHash(lhs) < qHash(rhs));
}

#endif // TRIGRAM_H
