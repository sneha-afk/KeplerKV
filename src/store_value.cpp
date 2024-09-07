#include "store_value.h"

#include "error_msgs.h"
#include "util.h"

#include <fstream>

std::vector<uint8_t> IntValue::serialize() const {
    std::vector<uint8_t> buf;
    buf.push_back('i');

    // Push back multiple bytes with insert()
    const uint8_t *val_ptr = reinterpret_cast<const uint8_t *>(&value_);
    buf.insert(buf.end(), val_ptr, val_ptr + sizeof(value_));
    return buf;
}

void IntValue::deserialize(std::ifstream &fp) {
    fp.read(reinterpret_cast<char *>(&value_), sizeof(value_));
}

std::vector<uint8_t> FloatValue::serialize() const {
    std::vector<uint8_t> buf;
    buf.push_back('f');

    const uint8_t *val_ptr = reinterpret_cast<const uint8_t *>(&value_);
    buf.insert(buf.end(), val_ptr, val_ptr + sizeof(value_));
    return buf;
}

void FloatValue::deserialize(std::ifstream &fp) {
    fp.read(reinterpret_cast<char *>(&value_), sizeof(value_));
}

// Strings are serialized as [ss][size][string]
std::vector<uint8_t> StringValue::serialize() const {
    std::vector<uint8_t> buf;
    buf.push_back('s');
    buf.push_back('s');

    const size_t &strSize = value_.size();
    const uint8_t *size_ptr = reinterpret_cast<const uint8_t *>(&strSize);
    buf.insert(buf.end(), size_ptr, size_ptr + sizeof(strSize));

    const uint8_t *val_ptr = reinterpret_cast<const uint8_t *>(&value_);
    buf.insert(buf.end(), val_ptr, val_ptr + strSize);
    return buf;
}

void StringValue::deserialize(std::ifstream &fp) {
    size_t strSize;
    fp.read(reinterpret_cast<char *>(&strSize), sizeof(strSize));
    std::string sval(strSize, '\0');

    fp.read(&sval[0], strSize);
    value_ = sval;
}

// Identifiers are serialized as [si][size][string]
std::vector<uint8_t> IdentifierValue::serialize() const {
    std::vector<uint8_t> buf = StringValue::serialize();
    buf[1] = 'i';
    return buf;
}

// Lists are serialized as [l][num elements][e1|e2|...|en|]
std::vector<uint8_t> ListValue::serialize() const {
    std::vector<uint8_t> buf;
    buf.push_back('l');

    const size_t &numElem = value_.size();
    const uint8_t *size_ptr = reinterpret_cast<const uint8_t *>(&numElem);
    buf.insert(buf.end(), size_ptr, size_ptr + sizeof(numElem));

    for (const auto &item : value_) {
        std::vector<uint8_t> bytes = item->serialize();
        buf.insert(buf.end(), bytes.begin(), bytes.end());
    }
    return buf;
}

void ListValue::deserialize(std::ifstream &fp) {
    size_t numVals;
    fp.read(reinterpret_cast<char *>(&numVals), sizeof(size_t));

    std::vector<StoreValueSP> lst = std::vector<StoreValueSP>();
    for (size_t i = 0; i < numVals; i++) {
        StoreValueSP valsp = fromFile(fp);
        lst.push_back(valsp);
    }
    value_ = lst;
}

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

// Writes the serialized format of this StoreValue into file.
void StoreValue::toFile(std::ofstream &fp) const {
    std::vector<uint8_t> bytes = serialize();
    fp.write(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

// Reads a StoreValue from the file, assuming the file is a valid KEPLER-SAVE.
StoreValueSP StoreValue::fromFile(std::ifstream &fp) {
    char type;
    fp.read(&type, sizeof(char));

    switch (type) {
        case 'i': {
            std::shared_ptr<IntValue> intVal = std::make_shared<IntValue>();
            intVal->deserialize(fp);
            return intVal;
        }
        case 'f': {
            std::shared_ptr<FloatValue> floatVal = std::make_shared<FloatValue>();
            floatVal->deserialize(fp);
            return floatVal;
        }
        case 's': {
            char strType;
            fp.read(&strType, sizeof(char));

            if (strType == 's') {
                std::shared_ptr<StringValue> strVal = std::make_shared<StringValue>();
                strVal->deserialize(fp);
                return strVal;
            } else if (strType == 'i') {
                std::shared_ptr<IdentifierValue> idVal = std::make_shared<IdentifierValue>();
                idVal->deserialize(fp);
                return idVal;
            } else
                throw RuntimeErr(UNK_SAVE_ITEM);
            break;
        }
        case 'l': {
            std::shared_ptr<ListValue> listVal = std::make_shared<ListValue>();
            listVal->deserialize(fp);
            return listVal;
        }
        default: throw RuntimeErr(UNK_SAVE_ITEM); break;
    }

    return nullptr;
}
