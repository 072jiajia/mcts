#pragma once
#include "../utils.h"
#include "selection.h"
#include "simulation.h"
#include "policy.h"

#define NODE_SEARCHER_STRATEGY(T, name)             \
public:                                             \
    inline auto name(T new_##name)->decltype(*this) \
    {                                               \
        delete this->name##_;                       \
        this->name##_ = new_##name;                 \
        return *this;                               \
    }                                               \
                                                    \
private:                                            \
    T name##_;

class MCTSNode_;

class NodeSearcher_
{
public:
    virtual ~NodeSearcher_() = default;
    virtual MCTSNode_ *CreateNode(Game *state) const = 0;
    virtual bool DoesSupportTreeParallel() const = 0;
    virtual void SearchOnce(MCTSNode_ *node, const Game *state) const = 0;
};

class MCTSNodeSearcher : public NodeSearcher_
{
public:
    MCTSNodeSearcher(SelectionStrategy *selection = nullptr,
                     SimulationStrategy *simulation = nullptr);
    ~MCTSNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    void SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    NODE_SEARCHER_STRATEGY(SelectionStrategy *, selection);
    NODE_SEARCHER_STRATEGY(SimulationStrategy *, simulation);
};

class MCTSNodeCSSearcher : public NodeSearcher_
{
public:
    MCTSNodeCSSearcher(SelectionStrategy *selection = nullptr,
                       SimulationStrategy *simulation = nullptr);
    ~MCTSNodeCSSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    void SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    NODE_SEARCHER_STRATEGY(SelectionStrategy *, selection);
    NODE_SEARCHER_STRATEGY(SimulationStrategy *, simulation);
};

class RaveNodeSearcher : public NodeSearcher_
{
public:
    RaveNodeSearcher(SelectionStrategy *selection = nullptr,
                     SimulationStrategy *simulation = nullptr);
    ~RaveNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    void SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    NODE_SEARCHER_STRATEGY(SelectionStrategy *, selection);
    NODE_SEARCHER_STRATEGY(SimulationStrategy *, simulation);
};

class MutexNodeSearcher : public NodeSearcher_
{
public:
    MutexNodeSearcher(SelectionStrategy *selection = nullptr,
                      SimulationStrategy *simulation = nullptr);
    ~MutexNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    void SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    NODE_SEARCHER_STRATEGY(SelectionStrategy *, selection);
    NODE_SEARCHER_STRATEGY(SimulationStrategy *, simulation);
};

class PolicyNodeSearcher : public NodeSearcher_
{
public:
    PolicyNodeSearcher(SelectionStrategy *selection = nullptr,
                       SimulationStrategy *simulation = nullptr,
                       PolicyStrategy *policy = nullptr);
    ~PolicyNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    void SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    NODE_SEARCHER_STRATEGY(SelectionStrategy *, selection);
    NODE_SEARCHER_STRATEGY(SimulationStrategy *, simulation);
    NODE_SEARCHER_STRATEGY(PolicyStrategy *, policy);
};
