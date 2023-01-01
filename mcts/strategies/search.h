#pragma once
#include "../utils.h"
#include "selection.h"
#include "simulation.h"

class MCTSNode_;

class SearchStrategy
{
public:
    virtual ~SearchStrategy() = default;
    virtual std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const = 0;
};

class MCTSSearch : public SearchStrategy
{
public:
    MCTSSearch(SelectionStrategy *, SimulationStrategy *);
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MCTSCopyStateSearch : public SearchStrategy
{
public:
    MCTSCopyStateSearch(SelectionStrategy *, SimulationStrategy *);
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class RaveSearch : public SearchStrategy
{
public:
    RaveSearch(SelectionStrategy *, SimulationStrategy *);
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MutexSearch : public SearchStrategy
{
public:
    MutexSearch(SelectionStrategy *, SimulationStrategy *);
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class PolicySearch : public SearchStrategy
{
public:
    PolicySearch(SelectionStrategy *, SimulationStrategy *);
    std::vector<float> *SearchOnce(MCTSNode_ *node, Game *state) const;

private:
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};
