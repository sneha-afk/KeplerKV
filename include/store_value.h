#pragma once

#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

// using StoreValueVar = std::variant<int, float, std::string, std::vector<StoreValueSP>>;

enum class ValueType { INT, FLOAT, STRING, LIST, IDENTIIFER };

class StoreValue {
public:
    virtual ~StoreValue() = default;

    virtual void serialize(std::ofstream &fp);
    virtual void deserialize(std::ifstream &fp);

    virtual ValueType getValueType() const = 0;
    virtual std::size_t size() const = 0;
    virtual std::string string() const = 0;
    friend std::ostream &operator<<(std::ostream &os, const StoreValue &s) {
        os << s.string();
        return os;
    };
};

using StoreValueSP = std::shared_ptr<StoreValue>;

class IntValue : public StoreValue {
public:
    IntValue(int i)
        : value_(i) {};

    int getValue() { return value_; }

    ValueType getValueType() { return ValueType::INT; }
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

    ValueType getValueType() { return ValueType::FLOAT; }
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

    ValueType getValueType() { return ValueType::STRING; }
    std::size_t size() const override { return sizeof(value_); }
    std::string string() const override { return "str: " + value_; }

private:
    std::string value_;
};

class IdentifierValue : public StoreValue {
public:
    IdentifierValue(std::string s)
        : value_(s) {};

    std::string getValue() { return value_; }

    ValueType getValueType() { return ValueType::IDENTIIFER; }
    std::size_t size() const override { return sizeof(value_); }
    std::string string() const override { return "id:" + value_; }

private:
    std::string value_;
};

class ListValue : public StoreValue {
public:
    ListValue()
        : value_(std::vector<StoreValueSP>()) {};
    ListValue(std::vector<StoreValueSP> &l)
        : value_(l) {};

    std::vector<StoreValueSP> &getValue() { return value_; }

    ValueType getValueType() { return ValueType::LIST; }
    std::size_t size() const override;
    std::string string() const override;

    void append(StoreValueSP item) { value_.push_back(item); }
    void prepend(StoreValueSP item) { value_.insert(value_.begin(), item); }

private:
    std::vector<StoreValueSP> value_;
};
