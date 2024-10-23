#pragma once

#include "store_value.h"

#include <string>
#include <unordered_map>
#include <unordered_set>

static constexpr unsigned int STORE_MIN_SIZE = 256;

class Store {
public:
    Store();

    void set(const std::string &, StoreValueSP);
    StoreValueSP get(const std::string &) const;
    bool del(const std::string &);
    bool update(const std::string &, StoreValueSP);
    StoreValueSP resolve(const std::string &, bool resolveIdentsInList = false) const;
    void rename(const std::string &, const std::string &);
    std::vector<std::string> search(const std::string &) const;

    bool contains(const std::string &key) const;

    void saveToFile(const std::string &) const;
    void loadFromFile(const std::string &);

    inline size_t size() const { return map_.size(); }

    std::unordered_map<std::string, StoreValueSP>::iterator begin() { return map_.begin(); }
    std::unordered_map<std::string, StoreValueSP>::const_iterator begin() const {
        return map_.begin();
    }
    std::unordered_map<std::string, StoreValueSP>::iterator end() { return map_.end(); }
    std::unordered_map<std::string, StoreValueSP>::const_iterator end() const { return map_.end(); }

private:
    std::unordered_map<std::string, StoreValueSP> map_;

    StoreValueSP resolveRecur_(const std::string &, std::unordered_set<std::string> &,
        bool resolveIdentsInList = false) const;
};
