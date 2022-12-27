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
    VirtualLossTree(Game *, int);
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
    MCTSMutexNode *root_;
    pthread_t *threads_;
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