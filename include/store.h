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
    StoreValueSP resolve(const std::string &);

    inline bool contains(const std::string &);

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

    StoreValueSP resolveRecur_(const std::string &, std::unordered_set<std::string> &);
};
