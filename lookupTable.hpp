#include "bitboard.hpp"
#include<vector>
#include <string.h>
#pragma once

class LookupTable
{
private:
    struct HashMap {
        uint64_t key : 58; 
        uint8_t value : 8; 
    };

    std::vector<HashMap> map;

    unsigned int index(uint64_t key) const{
        return key % map.size(); // Simple modulo hash function
    }
public:
    LookupTable(unsigned int size): map(size) {};

    void reset() {
        memset(map.data(), 0, map.size() * sizeof(HashMap));
    }

    void insert(uint64_t key, uint8_t value) {
        unsigned int idx = index(key);
        map[idx].key = key;
        map[idx].value = value;
    }

    uint8_t get(uint64_t key) const {
        unsigned int idx = index(key);
        if (map[idx].key == key) {
            return map[idx].value;
        }
        return 0; // Not found
    }
};
