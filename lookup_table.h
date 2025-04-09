//
// Created by min on 4/6/2025.
//

#ifndef LOOKUP_TABLE_H
#define LOOKUP_TABLE_H
#include <cstdint>
#include <cstring>
#include <memory>
#include <vector>

constexpr size_t TABLE_MAX_SIZE = (1 << 23) + 9; // prime value minimises hash collisions

struct Entry {
    uint32_t key = 0;
    uint8_t value = 0;

    Entry() = default;
};

class Storage {
public:
    Storage() : entries(TABLE_MAX_SIZE) {}

    void set(uint64_t key, uint8_t value) {
        Entry entry;
        entry.key = static_cast<uint32_t>(key);
        entry.value = value;

        size_t index = static_cast<size_t>(key) % entries.size();
        entries[index] = entry;
    }

    uint8_t get(uint64_t key) const {
        size_t index = static_cast<size_t>(key) % entries.size();
        const Entry& entry = entries[index];
        return (entry.key == static_cast<uint32_t>(key)) ? entry.value : 0;
    }

private:
    std::vector<Entry> entries;
};

class LookupTable {
public:
    LookupTable() : storage(std::make_shared<Storage>()) {}

    void set(uint64_t key, uint8_t value) {
        storage->set(key, value);
    }

    uint8_t get(uint64_t key) const {
        return storage->get(key);
    }

private:
    std::shared_ptr<Storage> storage;
};

#endif //LOOKUP_TABLE_H
