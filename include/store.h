#pragma once

#include "store_value.h"
#include "syntax_tree.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

static constexpr unsigned int STORE_MIN_SIZE = 256;

class Store {
public:
    Store();

    void set(const std::string &, StoreValueSP);
    StoreValueSP get(const std::string &);
    bool del(const std::string &);
    bool update(const std::string &, StoreValueSP);
    StoreValueSP resolve(const std::string &, bool resolveIdentsInList = false);
    void rename(const std::string &, const std::string &);

    // Indicates whether the store conatains the key.
    inline bool contains(const std::string &key) { return map_.find(key) != map_.end(); }

    void saveToFile(const std::string &);
    void loadFromFile(const std::string &);

    size_t size() { return map_.size(); }

    // Store is iterable: https://stackoverflow.com/a/46432381
    std::unordered_map<std::string, StoreValueSP>::iterator begin() {
        return map_.begin();
    }
    std::unordered_map<std::string, StoreValueSP>::const_iterator begin() const {
        return map_.begin();
    }
    std::unordered_map<std::string, StoreValueSP>::iterator end() { return map_.end(); }
    std::unordered_map<std::string, StoreValueSP>::const_iterator end() const {
        return map_.end();
    }

private:
    std::unordered_map<std::string, StoreValueSP> map_;

    StoreValueSP resolveRecur_(const std::string &, std::unordered_set<std::string> &,
        bool resolveIdentsInList = false);
};
