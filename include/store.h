#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

static constexpr unsigned int STORE_MIN_SIZE = 256;

struct StoreValue;
using StoreValueSP = std::shared_ptr<StoreValue>;
using StoreValueType = std::variant<int, std::string, std::vector<StoreValueSP>>;

class StoreValue {
private:
    StoreValueType value_;
    std::string stringRecur_() const;

public:
    StoreValue(int v)
        : value_(v) {};
    StoreValue(const std::string &v)
        : value_(v) {};
    StoreValue(const std::vector<StoreValueSP> &v)
        : value_(v) {};

    std::string string() const;
    friend std::ostream &operator<<(std::ostream &os, const StoreValue &sv) {
        os << sv.string();
        return os;
    };
};

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
