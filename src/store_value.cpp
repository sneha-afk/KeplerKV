#include "store.h"

#include <stdexcept>

static const std::runtime_error VALUE_ERR
    = std::runtime_error("Error: could not parse value");

StoreValue::StoreValue(const ValueNodeSP &v) {
    std::vector<StoreValueSP> asValues;
    switch (v->valType) {
        case ValueNodeType::INT: value_ = std::get<int>(v->value); break;
        case ValueNodeType::FLOAT: value_ = std::get<float>(v->value); break;
        case ValueNodeType::STRING: value_ = std::get<std::string>(v->value); break;
        case ValueNodeType::LIST:
            auto &list = std::get<std::vector<ValueNodeSP>>(v->value);
            asValues = std::vector<StoreValueSP>();
            for (const auto &node : list)
                asValues.push_back(std::make_shared<StoreValue>(node));
            value_ = asValues;
            break;
        default: throw VALUE_ERR;
    }
}

std::string stringList_(const std::vector<StoreValueSP> &arg) {
    std::string res = "list: [";
    for (size_t i = 0; i < arg.size(); i++) {
        res += arg[i]->string();
        if (i < arg.size() - 1) res += ", ";
    }
    res += "]";
    return res;
}

/**
 * https://en.cppreference.com/w/cpp/utility/variant/visit
 * https://dzone.com/articles/how-to-use-stdvisit-with-multiple-variants
 */
std::string StoreValue::stringRecur_() const {
    return std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<T, int>)
                return "int: " + std::to_string(arg);
            else if constexpr (std::is_same_v<T, float>)
                return "float: " + std::to_string(arg);
            else if constexpr (std::is_same_v<T, std::string>)
                return "str: " + arg;
            else if constexpr (std::is_same_v<T, std::vector<StoreValueSP>>)
                return stringList_(arg);
        },
        value_);
}

std::string StoreValue::string() const { return stringRecur_(); }
