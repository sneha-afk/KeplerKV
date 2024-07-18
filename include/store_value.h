#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

class StoreValue;
using StoreValueSP = std::shared_ptr<StoreValue>;
using StoreValueVar = std::variant<int, float, std::string, std::vector<StoreValueSP>>;

enum class StoreValueType { INT, FLOAT, STRING, LIST, IDENTIIFER };

class StoreValue {
private:
    StoreValueVar value_;
    StoreValueType type_;

public:
    StoreValue()
        : value_(0)
        , type_(StoreValueType::INT) {};
    StoreValue(int v)
        : value_(v)
        , type_(StoreValueType::INT) {};
    StoreValue(float v)
        : value_(v)
        , type_(StoreValueType::FLOAT) {};
    StoreValue(const std::string &v)
        : value_(v)
        , type_(StoreValueType::STRING) {};
    StoreValue(const std::string &v, bool is_ident)
        : value_(v) {
        if (is_ident)
            type_ = StoreValueType::IDENTIIFER;
        else
            type_ = StoreValueType::STRING;
    };
    StoreValue(const std::vector<StoreValueSP> &v)
        : value_(v)
        , type_(StoreValueType::LIST) {};

    // What type of value is this?
    bool isInt() const { return type_ == StoreValueType::INT; }
    bool isFloat() const { return type_ == StoreValueType::FLOAT; }
    bool isString() const { return type_ == StoreValueType::STRING; }
    bool isIdent() const { return type_ == StoreValueType::IDENTIIFER; }
    bool isList() const { return type_ == StoreValueType::LIST; }

    // Retrieve value, use in conjunction wtih getType()
    int getInt() const;
    float getFloat() const;
    const std::string &getString() const;
    const std::vector<StoreValueSP> &getList() const;
    std::vector<StoreValueSP> &getModifiableList();

    StoreValueType getType() const { return type_; }

    std::string string() const;
    friend std::ostream &operator<<(std::ostream &os, const StoreValue &sv) {
        os << sv.string();
        return os;
    };
};
