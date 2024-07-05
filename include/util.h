#pragma once

#include <string>
#include <unordered_map>

// Returns the vlaue of the key in the map if it exists, else returns the provided default
template <typename K, typename V>
V mapGet(const std::unordered_map<K, V> &map, const K &key, const V &dft) {
    auto found = map.find(key);
    if (found != map.end()) return found->second;
    return dft;
}
    