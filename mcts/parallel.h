#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "utils.h"
#include "node.h"
#include "tree.h"
#include "strategies/simulation.h"
#include "strategies/selection.h"
#include "strategies/timecontrol.h"
#include "strategies/decision.h"

struct MCTSThreadInput
{
    MCTSThreadInput(Game *b,
                    MCTSNode_ *root,
                    TimeControlStrategy *time_controller,
                    SelectionStrategy *selection_strategy,
                    SimulationStrategy *simulation_strategy)
    {
        b_ = b;
        root_ = root;
        time_controller_ = time_controller;
        selection_strategy_ = selection_strategy;
        simulation_strategy_ = simulation_strategy;
    }

    Game *b() { return b_; }
    MCTSNode_ *root() { return root_; }
    TimeControlStrategy *time_controller() { return time_controller_; }
    SelectionStrategy *selection_strategy() { return selection_strategy_; }
    SimulationStrategy *simulation_strategy() { return simulation_strategy_; }

private:
    Game *b_;
    MCTSNode_ *root_;
    TimeControlStrategy *time_controller_;
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class ThreadRootParallel : public MCTSTree_
{
public:
    ThreadRootParallel(Game *, int);
    ~ThreadRootParallel();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    static void *LaunchSearchThread(void *);

    Game *state_;
    int num_threads_;
    MCTSNode **roots_;
    pthread_t *threads_;
};

class VirtualLossTree : public MCTSTree_
{
public:
    VirtualLossTree(MCTSNode_ *, Game *, int);
    ~VirtualLossTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    static void *LaunchSearchThread(void *);

    Game *state_;
    int num_threads_;
    MCTSNode_ *root_;
    pthread_t *threads_;
};

class ProcessRootParallel : public MCTSTree_
{
public:
    ProcessRootParallel(Game *, MCTSTree_ *, int);
    ~ProcessRootParallel();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    int num_processes_;
    MCTSTree_ *tree_;
    int action_size_;
    int shm_id_;
    int *shm_;
    int shm_value_id_;
    float *shm_value_;
};
