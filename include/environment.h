#pragma once

#include "environment_interface.h"
#include "store.h"
#include "syntax_tree.h"

#include <deque>
#include <iostream>

using WALType = std::deque<StoreCommandSP>;

class Environment : public EnvironmentInterface {
public:
    // Dummy env with a nullptr Store
    Environment()
        : silentMode_(false) { }
    Environment(Store *s_ptr)
        : store_(s_ptr)
        , silentMode_(false) { }

    void printToConsole(const std::string &s) override {
        if (!silentMode_) std::cout << s << std::endl;
    }
    void setSilentMode(bool s) { silentMode_ = s; }

    Store *getStore() override { return store_; }

    // Transaction handling
    std::size_t sizeWAL() override { return wal_.size(); }
    bool isWALEmpty() override { return wal_.empty(); }
    void clearWAL() override { wal_.clear(); }

    void addCommand(StoreCommandSP &s) override { wal_.push_back(s); }

    StoreCommandSP getNextCommand() override {
        StoreCommandSP cmd = wal_.front();
        wal_.pop_front();
        return cmd;
    }

    void executeAllWAL() override {
        while (!isWALEmpty()) {
            StoreCommandSP walCmd = getNextCommand();
            walCmd->execute(*store_);
        }
    }

private:
    Store *store_;
    WALType wal_;
    bool silentMode_;
};
