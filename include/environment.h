#pragma once

#include "environment_interface.h"
#include "store.h"
#include "syntax_tree.h"

#include <deque>

using WALType = std::deque<StoreCommandSP>;

class Environment : public EnvironmentInterface {
public:
    Environment(Store &s)
        : store_(s) { }

    void printToConsole(const std::string &s) override { std::cout << s << std::endl; }

    Store &getStore() override { return store_; }

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

    void executeAllWAL(Store &s) override {
        while (!isWALEmpty()) {
            StoreCommandSP walCmd = getNextCommand();
            walCmd->execute(s);
        }
    }

private:
    Store store_;
    WALType wal_;
};
