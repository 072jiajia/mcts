#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>
#include <map>

#include "node.h"

class RaveNode : public MCTSNode
{
    /*
     * Implementation of this paper (https://doi.org/10.1016/j.artint.2011.03.007)
     */
public:
    RaveNode();

    virtual ~RaveNode();

    void Expansion(Game *state);
    float SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, std::vector<int> &self_action, std::vector<int> &oppo_action);

    float rave_Q(int);
    float rave_N(int);

protected:
    std::map<int, std::pair<float, float>> rave_QN_;
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

    int MakeDecision();

private:
    Game *state_;
    RaveNode *root_;
};
