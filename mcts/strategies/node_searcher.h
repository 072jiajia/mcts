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
    virtual std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const = 0;
};

class MCTSNodeSearcher : public NodeSearcher_
{
public:
    MCTSNodeSearcher(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MCTSNodeCSSearcher : public NodeSearcher_
{
public:
    MCTSNodeCSSearcher(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class RaveNodeSearcher : public NodeSearcher_
{
public:
    RaveNodeSearcher(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MutexNodeSearcher : public NodeSearcher_
{
public:
    MutexNodeSearcher(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class PolicyNodeSearcher : public NodeSearcher_
{
public:
    PolicyNodeSearcher(SelectionStrategy *, SimulationStrategy *, PolicyStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, const Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
    PolicyStrategy *policy_strategy_;
};
