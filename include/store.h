#pragma once

#include "store_value.h"
#include "syntax_tree.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

static constexpr unsigned int STORE_MIN_SIZE = 256;

class Store {
private:
    std::unordered_map<std::string, StoreValueSP> map_;

public:
    Store();

    void set(const std::string &, StoreValueSP);
    StoreValueSP get(const std::string &);
    bool del(const std::string &);
    bool update(const std::string &, StoreValueSP);

    inline bool contains(const std::string &);

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
};
