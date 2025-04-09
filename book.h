#pragma once
#include <fstream>
#include <vector>
#include <memory>
#include <cstdint>
#include <optional>
#include <algorithm>

const std::string DATABASE_PATH = "opening_database.bin";
const std::string TEMP_FILE_PATH = "temp_positions.bin";
constexpr size_t DATABASE_DEPTH = 12;
constexpr size_t DATABASE_NUM_POSITIONS = 4200899;

inline uint32_t read_u32_be(std::istream& in) {
    uint8_t bytes[4];
    in.read(reinterpret_cast<char*>(bytes), 4);
    return (static_cast<uint32_t>(bytes[0]) << 24) |
           (static_cast<uint32_t>(bytes[1]) << 16) |
           (static_cast<uint32_t>(bytes[2]) << 8) |
           static_cast<uint32_t>(bytes[3]);
}


inline int8_t read_i8(std::istream& in) {
    char byte;
    in.read(&byte, 1);
    return static_cast<int8_t>(byte);
}

inline void write_i8(std::ostream& out, int8_t value) {
    char byte = static_cast<char>(value);
    out.write(&byte, 1);
}

class OpeningDatabaseStorage {
public:
    std::vector<uint32_t> positions;
    std::vector<int8_t> values;

    static std::shared_ptr<OpeningDatabaseStorage> load() {
        std::ifstream file(DATABASE_PATH, std::ios::binary);
        if (!file.is_open()) throw std::runtime_error("Failed to open " + DATABASE_PATH);

        auto storage = std::make_shared<OpeningDatabaseStorage>();
        storage->positions.resize(DATABASE_NUM_POSITIONS);
        storage->values.resize(DATABASE_NUM_POSITIONS);

        for (size_t i = 0; i < DATABASE_NUM_POSITIONS; ++i) {
            storage->positions[i] = read_u32_be(file);
            storage->values[i] = read_i8(file);
        }

        return storage;
    }

    [[nodiscard]] std::optional<int> get(uint32_t position_code) const {
        size_t step = DATABASE_NUM_POSITIONS - 1;
        size_t pos = step;
        int value = -99;

        while (step > 0) {
            step = (step != 1) ? ((step + (step & 1)) >> 1) : 0;

            uint32_t code = pos < positions.size() ? positions[pos] : 0;

            if (position_code < code) {
                pos = pos >= step ? pos - step : 0;
            } else if (position_code > code) {
                pos += step;
            } else {
                value = values[pos];
                break;
            }
        }

        return value != -99 ? std::optional<int>(value) : std::nullopt;
    }
};

class OpeningDatabase {
private:
    std::shared_ptr<OpeningDatabaseStorage> storage;

public:
    explicit OpeningDatabase(std::shared_ptr<OpeningDatabaseStorage> s) : storage(std::move(s)) {}

    static OpeningDatabase load() {
        return OpeningDatabase(OpeningDatabaseStorage::load());
    }

    [[nodiscard]] std::optional<int> get(const uint32_t position_code) const {
        return storage->get(position_code);
    }
};
