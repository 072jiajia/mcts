#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "utils.h"
#include "node.h"
#include "strategies/simulation.h"
#include "strategies/selection.h"
#include "strategies/timecontrol.h"
#include "strategies/decision.h"

class MCTSTree_
{
public:
    virtual ~MCTSTree_(){};
    virtual float GetTotalSimulationCount() = 0;
    virtual void Search(SelectionStrategy *, SimulationStrategy *, TimeControlStrategy *) = 0;
    virtual int MakeDecision(DecisionStrategy *) = 0;
    virtual std::vector<int> GetFrequencies() = 0;
    virtual std::vector<float> GetValues() = 0;
};

class MCTSTree : public MCTSTree_
{
public:
    MCTSTree(Game *);
    ~MCTSTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    Game *state_;
    MCTSNode *root_;
};

class MCTSTreeCS : public MCTSTree_
{
public:
    MCTSTreeCS(Game *);
    ~MCTSTreeCS();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    Game *state_;
    MCTSNodeCS *root_;
};

class RaveTree : public MCTSTree_
{
public:
    RaveTree(Game *state);
    ~RaveTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *selection_strategy,
                SimulationStrategy *simulation_strategy,
                TimeControlStrategy *time_controller);

    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    Game *state_;
    RaveNode *root_;
};
