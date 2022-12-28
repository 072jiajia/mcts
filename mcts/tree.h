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
    MCTSTree(MCTSNode_ *, Game *);
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
    MCTSNode_ *root_;
};

class MCTSTreeCS : public MCTSTree_
{
public:
    MCTSTreeCS(MCTSNode_ *, Game *);
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
    MCTSNode_ *root_;
};
