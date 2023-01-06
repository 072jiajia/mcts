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
    virtual void MoveRoot(int index) { throw std::runtime_error("void MCTSTree_::MoveRoot() not implemented"); }
    virtual Game *GetState() = 0;
};

class SingleTree : public MCTSTree_
{
public:
    SingleTree(MCTSNode_ *, const Game *);
    ~SingleTree();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

    void MoveRoot(int index);
    Game *GetState();

private:
    Game *state_;
    MCTSNode_ *root_;
};

struct MCTSThreadInput
{
    MCTSThreadInput(Game *state, MCTSNode_ *root,
                    TimeControlStrategy *time_controller,
                    NodeSearcher_ *search_strategy)
        : state_{state}, root_{root},
          time_controller_{time_controller},
          search_strategy_{search_strategy} {}

    Game *state() { return state_; }
    MCTSNode_ *root() { return root_; }
    TimeControlStrategy *time_controller() { return time_controller_; }
    NodeSearcher_ *search_strategy() { return search_strategy_; }

private:
    Game *state_;
    MCTSNode_ *root_;
    TimeControlStrategy *time_controller_;
    NodeSearcher_ *search_strategy_;
};

void *LaunchSearchThread(void *);

class RootParallel : public MCTSTree_
{
public:
    RootParallel(MCTSNode_ **, const Game *, int);
    ~RootParallel();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

    void MoveRoot(int index);
    Game *GetState();

private:
    Game *state_;
    int num_threads_;
    MCTSNode_ **roots_;
    pthread_t *threads_;
};

class TreeParallel : public MCTSTree_
{
public:
    TreeParallel(MCTSNode_ *, const Game *, int);
    ~TreeParallel();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

    void MoveRoot(int index);
    Game *GetState();

private:
    Game *state_;
    int num_threads_;
    MCTSNode_ *root_;
    pthread_t *threads_;
};

class ProcessParallel : public MCTSTree_
{
public:
    ProcessParallel(const Game *, MCTSTree_ *, int);
    ~ProcessParallel();
    float GetTotalSimulationCount();
    void Search(NodeSearcher_ *, TimeControlStrategy *);
    int MakeDecision(DecisionStrategy *);
    std::vector<int> GetFrequencies();
    std::vector<float> GetValues();

    void PrepareSHM();
    void ClearSHM();

    void MoveRoot(int index);
    Game *GetState();

private:
    int num_processes_;
    MCTSTree_ *tree_;
    Game *state_;

    bool shm_usable_;
    int action_size_;
    int shm_id_;
    int *shm_;
    int shm_value_id_;
    float *shm_value_;
};
