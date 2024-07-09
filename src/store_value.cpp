#include "store_value.h"

#include <stdexcept>

static const std::runtime_error WRONG_TYPE_ERR
    = std::runtime_error("Error: wrong getter used with StoreValue");

int StoreValue::getInt() const {
    if (isInt())
        return std::get<int>(value_);
    else
        throw WRONG_TYPE_ERR;
}

float StoreValue::getFloat() const {
    if (isFloat())
        return std::get<float>(value_);
    else
        throw WRONG_TYPE_ERR;
}

const std::string &StoreValue::getString() const {
    if (isString())
        return std::get<std::string>(value_);
    else
        throw WRONG_TYPE_ERR;
}

const std::vector<StoreValueSP> &StoreValue::getList() const {
    if (isList())
        return std::get<std::vector<StoreValueSP>>(value_);
    else
        throw WRONG_TYPE_ERR;
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
