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
    virtual void Search(NodeSearcher_ *, TimeControlStrategy *) = 0;
    virtual int MakeDecision(DecisionStrategy *) = 0;
    virtual std::vector<int> GetFrequencies() = 0;
    virtual std::vector<float> GetValues() = 0;
};

class SingleTree : public MCTSTree_
{
public:
    SingleTree(MCTSNode_ *, Game *);
    ~SingleTree();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    Game *state_;
    MCTSNode_ *root_;
};

struct MCTSThreadInput
{
    MCTSThreadInput(Game *b, MCTSNode_ *root,
                    TimeControlStrategy *time_controller,
                    NodeSearcher_ *search_strategy)
        : b_{b}, root_{root},
          time_controller_{time_controller},
          search_strategy_{search_strategy} {}

    Game *b() { return b_; }
    MCTSNode_ *root() { return root_; }
    TimeControlStrategy *time_controller() { return time_controller_; }
    NodeSearcher_ *search_strategy() { return search_strategy_; }

private:
    Game *b_;
    MCTSNode_ *root_;
    TimeControlStrategy *time_controller_;
    NodeSearcher_ *search_strategy_;
};

void *LaunchSearchThread(void *);

class ThreadParallel : public MCTSTree_
{
public:
    ThreadParallel(MCTSNode_ **, Game *, int);
    ~ThreadParallel();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    Game *state_;
    int num_threads_;
    MCTSNode_ **roots_;
    pthread_t *threads_;
};

class MultiThreadSingleTree : public MCTSTree_
{
public:
    MultiThreadSingleTree(MCTSNode_ *, Game *, int);
    ~MultiThreadSingleTree();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

private:
    Game *state_;
    int num_threads_;
    MCTSNode_ *root_;
    pthread_t *threads_;
};

class ProcessParallel : public MCTSTree_
{
public:
    ProcessParallel(Game *, MCTSTree_ *, int);
    ~ProcessParallel();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
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
