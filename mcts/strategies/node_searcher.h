#pragma once
#include "../utils.h"
#include "selection.h"
#include "simulation.h"
#include "policy.h"

class MCTSNode_;

class NodeSearcher_
{
public:
    virtual ~NodeSearcher_() = default;
    virtual MCTSNode_ *CreateNode(Game *state) const = 0;
    virtual bool DoesSupportTreeParallel() const = 0;
    virtual std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const = 0;
};

class MCTSNodeSearcher : public NodeSearcher_
{
public:
    MCTSNodeSearcher(SelectionStrategy *, SimulationStrategy *);
    ~MCTSNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_;
    SimulationStrategy *simulation_;
};

class MCTSNodeCSSearcher : public NodeSearcher_
{
public:
    MCTSNodeCSSearcher(SelectionStrategy *, SimulationStrategy *);
    ~MCTSNodeCSSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_;
    SimulationStrategy *simulation_;
};

class RaveNodeSearcher : public NodeSearcher_
{
public:
    RaveNodeSearcher(SelectionStrategy *, SimulationStrategy *);
    ~RaveNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_;
    SimulationStrategy *simulation_;
};

class MutexNodeSearcher : public NodeSearcher_
{
public:
    MutexNodeSearcher(SelectionStrategy *, SimulationStrategy *);
    ~MutexNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_;
    SimulationStrategy *simulation_;
};

class PolicyNodeSearcher : public NodeSearcher_
{
public:
    PolicyNodeSearcher(SelectionStrategy *, SimulationStrategy *, PolicyStrategy *);
    ~PolicyNodeSearcher();
    MCTSNode_ *CreateNode(Game *state) const;
    bool DoesSupportTreeParallel() const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_;
    SimulationStrategy *simulation_;
    PolicyStrategy *policy_;
};
