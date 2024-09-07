#pragma once

#include "util.h"

#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

enum class ValueType { INT, FLOAT, STRING, LIST, IDENTIIFER };

class StoreValue;
using StoreValueSP = std::shared_ptr<StoreValue>;

class StoreValue {
public:
    virtual ~StoreValue() = default;

    /* Values are serialized by type identiifer, size of the value, then raw data.
    * ex. a string "abc" may be stored as s|4|abc
    */
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual void deserialize(std::ifstream &) = 0;

    void toFile(std::ofstream &) const;
    static StoreValueSP fromFile(std::ifstream &);

    virtual ValueType getValueType() const = 0;
    virtual std::size_t size() const = 0;
    virtual std::string string() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const StoreValue &s) {
        os << s.string();
        return os;
    };
};

class IntValue : public StoreValue {
public:
    IntValue(int i)
        : value_(i) {};

    int getValue() { return value_; }

    std::vector<uint8_t> serialize() const override;
    void deserialize(std::ifstream &) override;

    ValueType getValueType() const override { return ValueType::INT; }
    std::size_t size() const override { return sizeof(value_); }
    std::string string() const override { return "int: " + std::to_string(value_); }

    void incr() { value_ += 1; }
    void decr() { value_ -= 1; }

private:
    int value_;
};

class FloatValue : public StoreValue {
public:
    FloatValue(float f)
        : value_(f) {};

    float getValue() { return value_; }

    std::vector<uint8_t> serialize() const override;
    void deserialize(std::ifstream &) override;

    ValueType getValueType() const override { return ValueType::FLOAT; }
    std::size_t size() const override { return sizeof(value_); }
    std::string string() const override { return "float: " + std::to_string(value_); }

    void incr() { value_ += 1; }
    void decr() { value_ -= 1; }

private:
    float value_;
};

class StringValue : public StoreValue {
public:
    StringValue(std::string s)
        : value_(s) {};

    std::string &getValue() { return value_; }

    std::vector<uint8_t> serialize() const override;
    void deserialize(std::ifstream &) override;

    ValueType getValueType() const override { return ValueType::STRING; }
    std::size_t size() const override { return sizeof(value_); }
    std::string string() const override { return "str: " + value_; }

protected:
    std::string value_;
};

class IdentifierValue : public StringValue {
public:
    IdentifierValue(std::string s)
        : StringValue(s) {};

    std::vector<uint8_t> serialize() const override;

    ValueType getValueType() const override { return ValueType::IDENTIIFER; }
    std::string string() const override { return "id:" + value_; }
};

class ListValue : public StoreValue {
public:
    ListValue()
        : value_(std::vector<StoreValueSP>()) {};
    ListValue(std::vector<StoreValueSP> &l)
        : value_(l) {};
    ListValue(std::vector<StoreValueSP> &&l)
        : value_(std::move(l)) {};

    std::vector<StoreValueSP> &getValue() { return value_; }

    std::vector<uint8_t> serialize() const override;
    void deserialize(std::ifstream &) override;

    ValueType getValueType() const override { return ValueType::LIST; }
    std::size_t size() const override;
    std::string string() const override;

    void append(StoreValueSP item) { value_.push_back(item); }
    void prepend(StoreValueSP item) { value_.insert(value_.begin(), item); }

private:
    std::vector<StoreValueSP> value_;
};

using IntValueSP = std::shared_ptr<IntValue>;
using FloatValueSP = std::shared_ptr<FloatValue>;
using StringValueSP = std::shared_ptr<StringValue>;
using IdentifierValueSP = std::shared_ptr<IdentifierValue>;
using ListValueSP = std::shared_ptr<ListValue>;
