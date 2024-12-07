#include "store.h"

#include "error_msgs.h"
#include "file_io_macros.h"
#include "util.h"

#include <fstream>
#include <iostream>
#include <ostream>
#include <regex>

Store::Store() {
    map_ = std::unordered_map<std::string, StoreValueSP>();
    map_.reserve(STORE_MIN_SIZE);
}

// Indicates whether the store contains the key.
bool Store::contains(const std::string &key) const { return map_.find(key) != map_.end(); }

// Inserts a new key into the map, or updates the value if it exists.
void Store::set(const std::string &key, StoreValueSP value) { map_[key] = value; }

// Returns the key's value, or nullptr if it is not present.
StoreValueSP Store::get(const std::string &key) const {
    return mapGet(map_, key, (StoreValueSP) nullptr);
}

// Erases a key from the map, no effect if it is not present. Returns indication whether any deletion occurred.
bool Store::del(const std::string &key) { return map_.erase(key); }

// Updates a key's value. Returns true if updated, false if the key does not exist.
bool Store::update(const std::string &key, StoreValueSP value) {
    if (!contains(key)) return false;
    map_[key] = value;
    return true;
}

// Resolves recursive references (keys storing other keys) until a base value is reached.
// Essentially, a recursive GET command for when the user wants to unpack a key-chain.
StoreValueSP Store::resolve(const std::string &key, bool resolveIdentsInList) const {
    // Set of keys that have been explored to prevent circular references
    std::unordered_set<std::string> seen;
    return resolveRecur_(key, seen, resolveIdentsInList);
}

StoreValueSP Store::resolveRecur_(
    const std::string &key, std::unordered_set<std::string> &seen, bool resolveIdentsInList) const {
    // If a key is being searched for again, there is a circular ref
    if (seen.count(key)) throw RuntimeErr(CIRCULAR_REF);
    seen.insert(key);

    StoreValueSP found = get(key);
    if (!found) return nullptr;

    // If another identifier is found, continue down the chain
    if (found->getValueType() == ValueType::IDENTIFIER) {
        IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(found);
        return resolveRecur_(idNode->getValue(), seen, resolveIdentsInList);
    }

    // Resolve list elements (in case there are identifiers) only if requested (makes a copy)
    if (found->getValueType() == ValueType::LIST && resolveIdentsInList) {
        ListValueSP listNode = std::dynamic_pointer_cast<ListValue>(found);

        std::vector<StoreValueSP> resolvedL = std::vector<StoreValueSP>(listNode->getValue());
        for (std::size_t i = 0; i < resolvedL.size(); i++) {
            if (!resolvedL[i]) continue;
            if (resolvedL[i]->getValueType() == ValueType::IDENTIFIER) {
                // Each element should inherit parent history
                std::unordered_set<std::string> newSeen = std::unordered_set<std::string>(seen);

                IdentifierValueSP idNode = std::dynamic_pointer_cast<IdentifierValue>(resolvedL[i]);
                resolvedL[i] = resolveRecur_(idNode->getValue(), newSeen, resolveIdentsInList);
            }
        }
        return std::make_shared<ListValue>(resolvedL);
    }

    return found;
}

// Renames a value's key. WARNING: if `newName` was already present in the store, its value will be overwritten.
void Store::rename(const std::string &oldName, const std::string &newName) {
    StoreValueSP val = get(oldName);
    if (!val) return;

    // Delete old key, insert again
    map_.erase(oldName);
    map_[newName] = val;
}

// Searches for keys matching the given regex pattern.
std::vector<std::string> Store::search(const std::string &regexPattern) const {
    std::vector<std::string> keys;
    std::regex re(regexPattern);

    for (const auto &pair : map_) {
        if (std::regex_match(pair.first, re)) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

// Serializes the Store into binary at the filename specified.
void Store::saveToFile(const std::string &filename) const {
    std::ofstream fp;
    fp.open(filename, std::ios::out | std::ios::binary);
    if (!fp.is_open()) throw RuntimeErr(FAIL_OPEN_WRITE);

    fp.write(FILE_HEADER.data(), FILE_HEADER_SIZE);

    for (const auto &item : map_) {
        // [key size][key][value]
        const std::string &key = item.first;
        StoreValueSP val = item.second;

        size_t keySize = key.size();
        fp.write(reinterpret_cast<const char *>(&keySize), sizeof(keySize));
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
    if (!fp.is_open()) throw RuntimeErr(FAIL_OPEN_READ);

    // Confirm file header tag by filling empty buffer
    std::string expectHeader(FILE_HEADER_SIZE, '\0');
    fp.read(&expectHeader[0], FILE_HEADER_SIZE);
    if (expectHeader != FILE_HEADER) throw RuntimeErr(NOT_VALID_SAVE);

    while (fp.peek() != EOF) {
        size_t keySize;
        fp.read(reinterpret_cast<char *>(&keySize), sizeof(keySize));
        std::string key(keySize, '\0');
        fp.read(&key[0], keySize);
        fp.MV_FP_FORWARD;

        map_[key] = StoreValue::fromFile(fp);
    }
    fp.close();
}
