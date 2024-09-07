#include "util.h"

std::string removeQuotations(const std::string &s) {
    if (!s.size() || !(s[0] == '"' || s[0] == '\'')) return s;

    std::string res = s;
    char quote = s[0];
    res.erase(res.find_first_of(quote), 1);
    res.erase(res.find_last_of(quote), 1);
    return res;
}
