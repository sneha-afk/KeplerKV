#include "store_value.h"

#include "error_msgs.h"
#include "util.h"

#include <fstream>

std::size_t ListValue::size() const {
    std::size_t totalSize = sizeof(value_);
    for (const auto &item : value_)
        totalSize += item->size();
    return totalSize;
}

// Getting the string() of list elements
std::string ListValue::string() const {
    std::string res = "list: [";

    for (size_t i = 0; i < value_.size(); i++) {
        if (value_[i])
            res += value_[i]->string();
        else
            res += "<nil>";

        if (i < value_.size() - 1) res += ", ";
    }
    res += "]";
    return res;
}

/**
 * Serializes this StoreValue in binary format with a type identifier, size of the value,
 * then the value itself.
 * ex. a string "abc" may be stored as s|4|abc
 */
void StoreValue::toFile(std::ofstream &fp) const {
    char type;
    if (std::holds_alternative<int>(value_)) {
        type = 'i';
        fp.WRITE_CHAR(type);

        const int &i = std::get<int>(value_);
        fp.write(reinterpret_cast<const char *>(&i), sizeof(int));
    } else if (std::holds_alternative<float>(value_)) {
        type = 'f';
        fp.WRITE_CHAR(type);

        const float &f = std::get<float>(value_);
        fp.write(reinterpret_cast<const char *>(&f), sizeof(float));
    } else if (std::holds_alternative<std::string>(value_)) {
        // [s][i or s][size][string]
        type = 's';
        fp.WRITE_CHAR(type);

        char strType = isIdent() ? 'i' : 's';
        fp.WRITE_CHAR(strType);

        const std::string &str = std::get<std::string>(value_);
        const size_t &strSize = str.size();
        fp.write(reinterpret_cast<const char *>(&strSize), sizeof(strSize));
        fp.write(str.data(), strSize);
    } else if (std::holds_alternative<std::vector<StoreValueSP>>(value_)) {
        // [l][num elements][e1|e2|...|en|]
        type = 'l';
        fp.WRITE_CHAR(type);

        const std::vector<StoreValueSP> &list = std::get<std::vector<StoreValueSP>>(value_);
        const size_t &numElem = list.size();
        fp.write(reinterpret_cast<const char *>(&numElem), sizeof(numElem));

        for (const auto &item : list)
            item->toFile(fp);
    }
}

// Reads a StoreValue from the file, assuming the file is a valid KEPLER-SAVE.
StoreValue StoreValue::fromFile(std::ifstream &fp) {
    char type;
    fp.read(&type, sizeof(char));

    switch (type) {
        case 'i':
            int ival;
            fp.read(reinterpret_cast<char *>(&ival), sizeof(int));
            return StoreValue(ival);
        case 'f':
            float fval;
            fp.read(reinterpret_cast<char *>(&fval), sizeof(float));
            return StoreValue(fval);
        case 's': {
            char strType;
            fp.read(&strType, sizeof(char));

            size_t strSize;
            fp.read(reinterpret_cast<char *>(&strSize), sizeof(strSize));
            std::string sval(strSize, '\0');

            fp.read(&sval[0], strSize);
            if (strType == 'i') return StoreValue(sval, true);
            return StoreValue(sval);
        }
        case 'l': {
            size_t numVals;
            fp.read(reinterpret_cast<char *>(&numVals), sizeof(size_t));

            std::vector<StoreValueSP> lst = std::vector<StoreValueSP>();
            for (size_t i = 0; i < numVals; i++) {
                StoreValueSP valsp = std::make_shared<StoreValue>(fromFile(fp));
                lst.push_back(valsp);
            }
            return StoreValue(lst);
        }
        default: throw RuntimeErr(UNK_SAVE_ITEM); break;
    }

    return StoreValue();
}
