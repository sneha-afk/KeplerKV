#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class StoreValue;
using StoreValueSP = std::shared_ptr<StoreValue>;
using StoreValueType = std::variant<int, float, std::string, std::vector<StoreValueSP>>;

class StoreValue {
private:
    StoreValueType value_;
    std::string stringRecur_() const;

public:
    StoreValue()
        : value_(0) {};
    StoreValue(int v)
        : value_(v) {};
    StoreValue(float v)
        : value_(v) {};
    StoreValue(const std::string &v)
        : value_(v) {};
    StoreValue(const std::vector<StoreValueSP> &v)
        : value_(v) {};

    // What type of value is this?
    bool isInt() const { return std::holds_alternative<int>(value_); }
    bool isFloat() const { return std::holds_alternative<float>(value_); }
    bool isString() const { return std::holds_alternative<std::string>(value_); }
    bool isList() const {
        return std::holds_alternative<std::vector<StoreValueSP>>(value_);
    }

    // Retrieve value, should be wrapped around isType() to avoid errors
    int getInt() const;
    float getFloat() const;
    const std::string &getString() const;
    const std::vector<StoreValueSP> &getList() const;
    std::vector<StoreValueSP> &getModifiableList();

    std::string string() const;
    friend std::ostream &operator<<(std::ostream &os, const StoreValue &sv) {
        os << sv.string();
        return os;
    };
};
