#pragma once

#include "syntax_tree.h"

class QuitCommand : public SystemCommand {
public:
    QuitCommand()
        : SystemCommand(CommandType::QUIT) { }
    virtual void execute(EnvironmentInterface &) const override;
};

class ClearCommand : public SystemCommand {
public:
    ClearCommand()
        : SystemCommand(CommandType::CLEAR) { }
    virtual void execute(EnvironmentInterface &) const override;
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

class BeginCommand : public SystemCommand {
public:
    BeginCommand()
        : SystemCommand(CommandType::BEGIN) { }
    void execute(EnvironmentInterface &) const override;
};

class CommitCommand : public SystemCommand {
public:
    CommitCommand()
        : SystemCommand(CommandType::COMMIT) { }
    void execute(EnvironmentInterface &) const override;
};

class RollbackCommand : public SystemCommand {
public:
    RollbackCommand()
        : SystemCommand(CommandType::ROLLBACK) { }
    void execute(EnvironmentInterface &) const override;
};
