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
