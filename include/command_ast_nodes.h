#pragma once

#include "syntax_tree.h"

class QuitCmdASTNode : public SystemCmdASTNode {
public:
    QuitCmdASTNode()
        : SystemCmdASTNode(CommandType::QUIT) { }
    virtual void execute() const override;
};

class ClearCmdASTNode : public SystemCmdASTNode {
public:
    ClearCmdASTNode()
        : SystemCmdASTNode(CommandType::CLEAR) { }
    virtual void execute() const override;
};

class SetCmdASTNode : public StoreCmdASTNode {
public:
    SetCmdASTNode()
        : StoreCmdASTNode(CommandType::SET) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class GetCmdASTNode : public StoreCmdASTNode {
public:
    GetCmdASTNode()
        : StoreCmdASTNode(CommandType::GET) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class ListCmdASTNode : public StoreCmdASTNode {
public:
    ListCmdASTNode()
        : StoreCmdASTNode(CommandType::LIST) { }
    virtual void execute(Store &) const override;
};

class DeleteCmdASTNode : public StoreCmdASTNode {
public:
    DeleteCmdASTNode()
        : StoreCmdASTNode(CommandType::DELETE) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class UpdateCmdASTNode : public StoreCmdASTNode {
public:
    UpdateCmdASTNode()
        : StoreCmdASTNode(CommandType::UPDATE) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class ResolveCmdASTNode : public StoreCmdASTNode {
public:
    ResolveCmdASTNode()
        : StoreCmdASTNode(CommandType::RESOLVE) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class SaveCmdASTNode : public StoreCmdASTNode {
public:
    SaveCmdASTNode()
        : StoreCmdASTNode(CommandType::SAVE) { }
    virtual void execute(Store &) const override;
};

class LoadCmdASTNode : public StoreCmdASTNode {
public:
    LoadCmdASTNode()
        : StoreCmdASTNode(CommandType::LOAD) { }
    virtual void execute(Store &) const override;
};

class RenameCmdASTNode : public StoreCmdASTNode {
public:
    RenameCmdASTNode()
        : StoreCmdASTNode(CommandType::RENAME) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class IncrCmdASTNode : public StoreCmdASTNode {
public:
    IncrCmdASTNode()
        : StoreCmdASTNode(CommandType::INCR) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class DecrCmdASTNode : public StoreCmdASTNode {
public:
    DecrCmdASTNode()
        : StoreCmdASTNode(CommandType::DECR) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class AppendCmdASTNode : public StoreCmdASTNode {
public:
    AppendCmdASTNode()
        : StoreCmdASTNode(CommandType::APPEND) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class PrependCmdASTNode : public StoreCmdASTNode {
public:
    PrependCmdASTNode()
        : StoreCmdASTNode(CommandType::PREPEND) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class SearchCmdASTNode : public StoreCmdASTNode {
public:
    SearchCmdASTNode()
        : StoreCmdASTNode(CommandType::SEARCH) { }
    virtual bool validate() const override;
    virtual void execute(Store &) const override;
};

class StatsCmdASTNode : public StoreCmdASTNode {
public:
    StatsCmdASTNode()
        : StoreCmdASTNode(CommandType::STATS) { }
    virtual void execute(Store &) const override;
};

class BeginCmdASTNode : public StoreCmdASTNode {
public:
    BeginCmdASTNode()
        : StoreCmdASTNode(CommandType::BEGIN) { }
    void execute(Store &s) const override {
        std::cout << T_BYLLW << "TRANSAC BEGIN" << T_RESET << std::endl;
    }
};

class CommitCmdASTNode : public StoreCmdASTNode {
public:
    CommitCmdASTNode()
        : StoreCmdASTNode(CommandType::COMMIT) { }
    void execute(Store &s) const override {
        std::cout << T_BYLLW << "TRANSAC COMMITTED" << T_RESET << std::endl;
    }
};

class RollbackCmdASTNode : public StoreCmdASTNode {
public:
    RollbackCmdASTNode()
        : StoreCmdASTNode(CommandType::ROLLBACK) { }
    void execute(Store &s) const override {
        (void) s;
        std::cout << T_BYLLW << "TRANSAC ROLLBACK" << T_RESET << std::endl;
    }
};
