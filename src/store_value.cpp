#include "store_value.h"

#include "error_msgs.h"
#include "util.h"

#include <fstream>

int StoreValue::getInt() const {
    if (isInt())
        return std::get<int>(value_);
    else
        throw RuntimeErr(SV_WRONG_TYPE);
}

float StoreValue::getFloat() const {
    if (isFloat())
        return std::get<float>(value_);
    else
        throw RuntimeErr(SV_WRONG_TYPE);
}

const std::string &StoreValue::getString() const {
    if (isString() || isIdent())
        return std::get<std::string>(value_);
    else
        throw RuntimeErr(SV_WRONG_TYPE);
}

const std::vector<StoreValueSP> &StoreValue::getList() const {
    if (isList())
        return std::get<std::vector<StoreValueSP>>(value_);
    else
        throw RuntimeErr(SV_WRONG_TYPE);
}

std::vector<StoreValueSP> &StoreValue::getModifiableList() {
    if (isList())
        return std::get<std::vector<StoreValueSP>>(value_);
    else
        throw RuntimeErr(SV_WRONG_TYPE);
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
 * Returns a string representation with the datatype and value of this StoreValue.
 * https://en.cppreference.com/w/cpp/utility/variant/visit
 * https://dzone.com/articles/how-to-use-stdvisit-with-multiple-variants
 */
std::string StoreValue::string() const {
    if (isIdent()) return "id: " + std::get<std::string>(value_);

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

/**
 * Serializes this StoreValue in binary format with a type identifier, size of the value,
 * then the value itself.
 * ex. a string "abc" may be stored as s|4|abc
 */
void StoreValue::toFile(std::ofstream &fp) const {
    char type;
    size_t size;
    if (std::holds_alternative<int>(value_)) {
        type = 'i';
        fp.WRITE_CHAR(type);
        fp.WRITE_DELIM;

        const int &i = std::get<int>(value_);
        fp.write(reinterpret_cast<const char *>(&i), sizeof(int));
    } else if (std::holds_alternative<float>(value_)) {
        type = 'f';
        fp.WRITE_CHAR(type);
        fp.WRITE_DELIM;

        const float &f = std::get<float>(value_);
        fp.write(reinterpret_cast<const char *>(&f), sizeof(float));
    } else if (std::holds_alternative<std::string>(value_)) {
        // s|size|string
        type = 's';
        fp.WRITE_CHAR(type);
        fp.WRITE_DELIM;

        const std::string &str = std::get<std::string>(value_);
        const size_t &strSize = str.size();
        fp.write(reinterpret_cast<const char *>(&strSize), sizeof(strSize));
        fp.WRITE_DELIM;
        fp.write(str.data(), strSize);
    } else if (std::holds_alternative<std::vector<StoreValueSP>>(value_)) {
        static const char DENOTE_L_START = '<';
        static const char DENOTE_L_END = '>';

        // l|numElem|e1|e2|...|en|
        type = 'l';
        fp.WRITE_CHAR(type);
        fp.WRITE_DELIM;

        const std::vector<StoreValueSP> &list
            = std::get<std::vector<StoreValueSP>>(value_);
        const size_t &numElem = list.size();
        fp.write(reinterpret_cast<const char *>(&numElem), sizeof(numElem));

        fp.WRITE_CHAR(DENOTE_L_START);
        for (const auto &item : list) {
            item->toFile(fp);
            fp.WRITE_DELIM;
        }
        fp.WRITE_CHAR(DENOTE_L_END);
    }
}
