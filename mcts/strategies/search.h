#pragma once
#include "../utils.h"
#include "selection.h"
#include "simulation.h"

class MCTSNode_;

class SearchStrategy
{
public:
    virtual ~SearchStrategy() = default;
    virtual MCTSNode_ *CreateNode(Game *state) const = 0;
    virtual std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const = 0;
};

class MCTSSearch : public SearchStrategy
{
public:
    MCTSSearch(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MCTSCopyStateSearch : public SearchStrategy
{
public:
    MCTSCopyStateSearch(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class RaveSearch : public SearchStrategy
{
public:
    RaveSearch(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MutexSearch : public SearchStrategy
{
public:
    MutexSearch(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class PolicySearch : public SearchStrategy
{
public:
    PolicySearch(SelectionStrategy *, SimulationStrategy *);
    MCTSNode_ *CreateNode(Game *state) const;
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};
