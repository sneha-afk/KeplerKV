#include "store.h"

Store::Store() {
    map_ = std::unordered_map<std::string, StoreValueSP>();
    map_.reserve(STORE_MIN_SIZE);
}

// Inserts a new key into the map, or updates the value if it exists.
void Store::set(const std::string &key, StoreValueSP value) { map_[key] = value; }

// Returns the key's value, or nullptr if it is not present.
StoreValueSP Store::get(const std::string &key) {
    auto found = map_.find(key);
    if (found == map_.end()) return nullptr;
    return found->second;
}

// Erases a key from the map, no effect if it is not present.
void Store::del(const std::string &key) { map_.erase(key); }

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
