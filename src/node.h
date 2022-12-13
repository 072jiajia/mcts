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

class MCTSNodeBase
{
public:
    float GetValueForCurrentPlayer();
    float GetTotalSimulationCount();
    float Q();
    float N();
    bool IsExpanded();
    std::vector<MCTSNodeBase *> GetChildren() const;
    MCTSNodeBase();
    ~MCTSNodeBase();
    int ChooseMoveWithMostFrequency();

protected:
    std::vector<MCTSNodeBase *> children_;
    float Q_;
    float N_;
    bool expanded_;
};

class MCTSNode : public MCTSNodeBase
{
public:
    Game *GetCurrentState();

    MCTSNode(Game *s);
    ~MCTSNode();
    void Expansion();

    float DoMonteCarloTreeSearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy);

protected:
    Game *state_;
};

class MCTSNodeV2 : public MCTSNodeBase
{
    /* The difference between MCTSNode & MCTSNodeV2 is that
     * instead of saving the state in the node,
     * MCTSNodeV2 takes a mutable state as an input and
     * does actions on that state while doing monte carlo tree search.
     */
public:
    MCTSNodeV2();

    ~MCTSNodeV2();

    void Expansion(Game *state);

    float DoMonteCarloTreeSearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy);

protected:
    std::vector<Action *> actions_;
};
