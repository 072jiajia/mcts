#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>
#include <map>

#include "node.h"

class RaveNode : public MCTSNodeV2
{
    /*
     * Implementation of this paper (https://doi.org/10.1016/j.artint.2011.03.007)
     */
public:
    RaveNode();

    ~RaveNode();

    void Expansion(Game *state);
    float DoMonteCarloTreeSearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, std::vector<int> &self_action, std::vector<int> &oppo_action);

    float rave_Q(int);
    float rave_N(int);

protected:
    std::map<int, std::pair<float, float>> rave_QN_;
};