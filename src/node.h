#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "utils.h"
#include "selection.h"
#include "simulation.h"

class MCTSNode_
{
public:
    float GetValueForCurrentPlayer();
    float GetTotalSimulationCount();
    float Q();
    float N();
    bool IsExpanded();
    std::vector<MCTSNode_ *> GetChildren() const;
    MCTSNode_();
    virtual ~MCTSNode_();
    int ChooseMoveWithMostFrequency();
    std::vector<int> GetFrequencies();

protected:
    float EvaluateGameOverNode(Game *);

    std::vector<MCTSNode_ *> children_;
    float Q_;
    float N_;
    bool expanded_;
};

class MCTSNodeCS : public MCTSNode_
{
public:
    Game *GetCurrentState();

    MCTSNodeCS(Game *s);
    virtual ~MCTSNodeCS();
    void Expansion();

    float DoMonteCarloTreeSearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy);

protected:
    Game *state_;
};

class MCTSNode : public MCTSNode_
{
    /* The difference between MCTSNodeCS & MCTSNode is that
     * instead of saving the state in the node,
     * MCTSNode takes a mutable state as an input and
     * does actions on that state while doing monte carlo tree search.
     */
public:
    MCTSNode();

    virtual ~MCTSNode();

    void Expansion(Game *state);

    float DoMonteCarloTreeSearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy);

protected:
    ActionList *actions_;
};
