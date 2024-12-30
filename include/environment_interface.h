/**
 * Dependency inversion needed to avoid a circular dependence of Environment <-> StoreCommand
 */
#pragma once

#include <memory>
#include <stdlib.h>
#include <string>

// Forward declarations
class Store;
class StoreCommand;
using StoreCommandSP = std::shared_ptr<StoreCommand>;

class EnvironmentInterface {
public:
    EnvironmentInterface()
        : running_(true)
        , transac_(false) { }

    virtual ~EnvironmentInterface() = default;

    void setRunning(bool r) { running_ = r; };
    bool isRunning() { return running_; };

    virtual void printToConsole(const std::string & = "", bool ignoreSilent = false) = 0;

    virtual Store *getStore() = 0;

    // Transaction handling
    void setTransacState(bool t) { transac_ = t; };
    bool inTransaction() { return transac_; };

    virtual void clearWAL() = 0;
    virtual std::size_t sizeWAL() = 0;
    virtual bool isWALEmpty() = 0;

    virtual void addCommand(StoreCommandSP &) = 0;
    virtual StoreCommandSP getNextCommand() = 0;
    virtual void executeAllWAL() = 0;

    void exitSuccess() { exit(EXIT_SUCCESS); }

protected:
    bool running_;
    bool transac_;
};
