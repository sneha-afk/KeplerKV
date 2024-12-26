#pragma once

#include "syntax_tree.h"

class QuitCommand : public SystemCommand {
public:
    QuitCommand()
        : SystemCommand(CommandType::QUIT) { }
    virtual void execute() const override;
};

class ClearCommand : public SystemCommand {
public:
    ClearCommand()
        : SystemCommand(CommandType::CLEAR) { }
    virtual void execute() const override;
};

class SetCommand : public StoreCommand {
public:
    SetCommand()
        : StoreCommand(CommandType::SET) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class GetCommand : public StoreCommand {
public:
    GetCommand()
        : StoreCommand(CommandType::GET) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class ListCommand : public StoreCommand {
public:
    ListCommand()
        : StoreCommand(CommandType::LIST) { }
    virtual void execute(Store &) const override;
};

class DeleteCommand : public StoreCommand {
public:
    DeleteCommand()
        : StoreCommand(CommandType::DELETE) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class UpdateCommand : public StoreCommand {
public:
    UpdateCommand()
        : StoreCommand(CommandType::UPDATE) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class ResolveCommand : public StoreCommand {
public:
    ResolveCommand()
        : StoreCommand(CommandType::RESOLVE) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class SaveCommand : public StoreCommand {
public:
    SaveCommand()
        : StoreCommand(CommandType::SAVE) { }
    virtual void execute(Store &) const override;
};

class LoadCommand : public StoreCommand {
public:
    LoadCommand()
        : StoreCommand(CommandType::LOAD) { }
    virtual void execute(Store &) const override;
};

class RenameCommand : public StoreCommand {
public:
    RenameCommand()
        : StoreCommand(CommandType::RENAME) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class IncrementCommand : public StoreCommand {
public:
    IncrementCommand()
        : StoreCommand(CommandType::INCR) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class DecrementCommand : public StoreCommand {
public:
    DecrementCommand()
        : StoreCommand(CommandType::DECR) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class AppendCommand : public StoreCommand {
public:
    AppendCommand()
        : StoreCommand(CommandType::APPEND) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class PrependCommand : public StoreCommand {
public:
    PrependCommand()
        : StoreCommand(CommandType::PREPEND) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class SearchCommand : public StoreCommand {
public:
    SearchCommand()
        : StoreCommand(CommandType::SEARCH) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class StatsCommand : public StoreCommand {
public:
    StatsCommand()
        : StoreCommand(CommandType::STATS) { }
    virtual void execute(Store &) const override;
};

class BeginCommand : public StoreCommand {
public:
    BeginCommand()
        : StoreCommand(CommandType::BEGIN) { }
    void execute(Store &s) const override {
        std::cout << T_BYLLW << "TRANSAC BEGIN" << T_RESET << std::endl;
    }
};

class CommitCommand : public StoreCommand {
public:
    CommitCommand()
        : StoreCommand(CommandType::COMMIT) { }
    void execute(Store &s) const override {
        std::cout << T_BYLLW << "TRANSAC COMMITTED" << T_RESET << std::endl;
    }
};

class RollbackCommand : public StoreCommand {
public:
    RollbackCommand()
        : StoreCommand(CommandType::ROLLBACK) { }
    void execute(Store &s) const override {
        (void) s;
        std::cout << T_BYLLW << "TRANSAC ROLLBACK" << T_RESET << std::endl;
    }
};
