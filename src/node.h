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
    virtual ~MCTSNode_(){};
    virtual float GetValueForCurrentPlayer() = 0;
    virtual float GetTotalSimulationCount() = 0;
    virtual float Q() = 0;
    virtual float N() = 0;
    virtual bool IsExpanded() = 0;
    virtual std::vector<MCTSNode_ *> GetChildren() const = 0;

    virtual int ChooseMoveWithMostFrequency() = 0;
    virtual std::vector<int> GetFrequencies() = 0;
};

class MCTSNodeImpl_ : public MCTSNode_
{
public:
    MCTSNodeImpl_();
    virtual ~MCTSNodeImpl_();
    float GetValueForCurrentPlayer();
    float GetTotalSimulationCount();
    float Q();
    float N();
    bool IsExpanded();
    std::vector<MCTSNode_ *> GetChildren() const;
    int ChooseMoveWithMostFrequency();
    std::vector<int> GetFrequencies();

protected:
    float EvaluateGameOverNode(Game *);

    std::vector<MCTSNode_ *> children_;
    float Q_;
    float N_;
    bool expanded_;
};

class MCTSNodeCS : public MCTSNodeImpl_
{
public:
    Game *GetCurrentState();

    MCTSNodeCS(Game *s);
    virtual ~MCTSNodeCS();
    void Expansion();

    float SearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy);

protected:
    Game *state_;
};

class MCTSNode : public MCTSNodeImpl_
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

    float SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy);

protected:
    ActionList *actions_;
};
