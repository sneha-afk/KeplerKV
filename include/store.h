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
    void del(const std::string &);
    bool update(const std::string &, StoreValueSP);

    inline bool contains(const std::string &);
};
