#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

using StoreValue = std::variant<int, float, std::string, std::vector<StoreValue>>;

class Store {
private:
    std::unordered_map<std::string, StoreValue> store_;

public:
    Store();

    void set(std::string &, StoreValue);
    StoreValue get(std::string &);
    bool del(std::string &);
    bool update(std::string &, StoreValue);
};
