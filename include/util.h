#pragma once

#include "terminal_colors.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

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

// void printItem(const std::pair<const std::string, StoreValueSP> &item) {
//     std::cout << T_BBLUE << item.first << T_RESET << " | " << *(item.second) << std::endl;
// }
