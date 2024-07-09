#include "store.h"


Store::Store() {
    map_ = std::vector<std::string, StoreValueSP>();
    map_.reserve(STORE_MIN_SIZE);
}
