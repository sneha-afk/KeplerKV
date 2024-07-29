#include "store.h"

#include "error_msgs.h"
#include "util.h"

#include <fstream>
#include <iostream>
#include <ostream>

Store::Store() {
    map_ = std::unordered_map<std::string, StoreValueSP>();
    map_.reserve(STORE_MIN_SIZE);
}

// Inserts a new key into the map, or updates the value if it exists.
void Store::set(const std::string &key, StoreValueSP value) { map_[key] = value; }

// Returns the key's value, or nullptr if it is not present.
StoreValueSP Store::get(const std::string &key) {
    return mapGet(map_, key, (StoreValueSP) nullptr);
}

// Erases a key from the map, no effect if it is not present. Returns indication whether any deletion occured.
bool Store::del(const std::string &key) { return map_.erase(key); }

// Updates a key's value. Returns true if updated, false if the key does not exist.
bool Store::update(const std::string &key, StoreValueSP value) {
    if (!contains(key)) return false;
    map_[key] = value;
    return true;
}

// Indicates whether the store conatains the key.
inline bool Store::contains(const std::string &key) {
    return map_.find(key) != map_.end();
}

// Resolves recursive references (keys storing other keys) until a base value is reached.
// Essentially, a recursive GET command for when the user wants to unpack a key-chain.
StoreValueSP Store::resolve(const std::string &key) {
    // Set of keys that have been explored to prevent circular references
    std::unordered_set<std::string> seen;
    return resolveRecur_(key, seen);
}

StoreValueSP Store::resolveRecur_(
    const std::string &key, std::unordered_set<std::string> &seen) {
    // If a key is being searched for again, there is a circluar ref
    if (seen.count(key)) throw CIRCULAR_REF;
    seen.insert(key);

    StoreValueSP found = get(key);
    if (!found) return nullptr;

    // If another identiifer is found, continue down the chain
    if (found->isIdent()) return resolveRecur_(found->getString(), seen);

    return found;
}

/**
 * Serializes the Store into binary at the filename specified.
 * 
 * https://www.eecs.umich.edu/courses/eecs380/HANDOUTS/cppBinaryFileIO-2.html
 * https://gist.github.com/molpopgen/9123133
 */
void Store::saveToFile(const std::string &filename) {
    std::ofstream fp;
    fp.open(filename, std::ios::out | std::ios::binary);
    if (!fp.is_open()) throw RuntimeErr("Error: failed to open file to save");

    fp.write(FILE_HEADER.data(), FILE_HEADER_SIZE);

    for (const auto &item : map_) {
        const std::string &key = item.first;
        StoreValueSP val = item.second;

        size_t keySize = key.size();
        fp.write(reinterpret_cast<const char *>(&keySize), sizeof(keySize));
        fp.WRITE_DELIM;
        fp.write(key.data(), keySize);
        fp.WRITE_DELIM;
        val->toFile(fp);
    }
    fp.close();
}

// Deserializes a binary KEPLER-SAVE file into a Store with all its data.
void Store::loadFromFile(const std::string &filename) {
    std::ifstream fp;
    fp.open(filename, std::ios::in | std::ios::binary);
    if (!fp.is_open()) throw RuntimeErr("Error: failed to open file to read");

    // Confirm file header tag by filling empty buffer
    std::string expectHeader(FILE_HEADER_SIZE, '\0');
    fp.read(&expectHeader[0], FILE_HEADER_SIZE);
    if (expectHeader != FILE_HEADER)
        throw RuntimeErr("Error: not a valid KEPLER-SAVE file");

    while (fp.peek() != EOF) {
        size_t keySize;
        fp.read(reinterpret_cast<char *>(&keySize), sizeof(keySize));
        fp.MV_FP_FORWARD;
        std::string key(keySize, '\0');
        fp.read(&key[0], keySize);
        fp.MV_FP_FORWARD;

        StoreValueSP val = std::make_shared<StoreValue>(StoreValue::fromFile(fp));
        map_[key] = val;
    }
    fp.close();
}
