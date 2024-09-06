#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

static const std::string FILE_HEADER = "KEPLERKV-SAVE|";
static const int FILE_HEADER_SIZE = (int) FILE_HEADER.size();
static std::string DEFAULT_SAVE_FILE = "default_kep_save";

static const char DELIMITER = '|';
#define WRITE_DELIM    write(&DELIMITER, sizeof(char))
#define WRITE_CHAR(c)  write(&c, sizeof(char))
#define MV_FP_FORWARD  seekg(1, std::ios::cur)
#define MV_FP_BACKWARD seekg(-1, std::ios::cur)

// Returns the vlaue of the key in the map if it exists, else returns the provided default
template <typename K, typename V>
V mapGet(const std::unordered_map<K, V> &map, const K &key, const V &dft) {
    auto found = map.find(key);
    if (found != map.end()) return found->second;
    return dft;
}

inline bool strContains(const std::string &s, const char &c) {
    return s.find(c) != std::string::npos;
}

// Removes quotation marks without affecting original string
std::string removeQuotations(const std::string &s);

// Provides common functionality to be used for string operations.
class StringHandler {
public:
    static void serializeToBuffer(std::vector<uint8_t> &, const std::string &s);
    static std::string deserializeFromFile(std::ifstream &);
};
