//
// Created by min on 4/6/2025.
//

#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H
#include <cstdint>
#include <cstring>
#include <vector>

using ull = uint64_t;

class LookupTable {
private:
    struct Entry {
        ull key;
        uint8_t score;
    };

    std::vector<Entry> table;

    unsigned int index(const ull key) const {
        return static_cast<unsigned int>(key % table.size());
    }

public:
    explicit LookupTable(size_t size) : table(size) {}

    void insert(const ull key, const uint8_t score) {
        const unsigned int idx = index(key);
        table[idx] = {key, score};
    }

    uint8_t find(const ull key) const {
        const unsigned int idx = index(key);
        if (table[idx].key == key) {
            return table[idx].score;
        }
        return 0;
    }

    void clear() {
        memset(table.data(), 0, table.size() * sizeof(Entry));
    }
};

#endif //LOOKUP_TABLE_H
