#include "util.h"

std::string removeQuotations(const std::string &s) {
    std::string res = s;
    size_t first_quote = res.find_first_of('"');
    if (first_quote != std::string::npos) {
        res.erase(res.find_last_of('"'), 1);
    } else {
        first_quote = res.find_first_of('\'');
        res.erase(res.find_last_of('\''), 1);
    }
    res.erase(first_quote, 1);
    return res;
}

void StringHandler::serializeToBuffer(std::vector<uint8_t> &buf, const std::string &s) {
    const size_t &strSize = s.size();
    const uint8_t *size_ptr = reinterpret_cast<const uint8_t *>(&strSize);
    buf.insert(buf.end(), size_ptr, size_ptr + sizeof(strSize));

    const uint8_t *val_ptr = reinterpret_cast<const uint8_t *>(&s);
    buf.insert(buf.end(), val_ptr, val_ptr + strSize);
}

std::string StringHandler::deserializeFromFile(std::ifstream &fp) {
    size_t strSize;
    fp.read(reinterpret_cast<char *>(&strSize), sizeof(strSize));
    std::string sval(strSize, '\0');

    fp.read(&sval[0], strSize);
    return sval;
}
