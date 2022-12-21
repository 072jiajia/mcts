#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "utils.h"
#include "node.h"
#include "selection.h"
#include "simulation.h"
#include "timecontrol.h"

class MCTSTree_
{
public:
    virtual ~MCTSTree_(){};
    virtual float GetTotalSimulationCount() = 0;
    virtual void Search(SelectionStrategy *, SimulationStrategy *, TimeControlStrategy *) = 0;
    virtual int MakeDecision() = 0;
    virtual std::vector<int> GetFrequencies() = 0;
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
    int MakeDecision();
    std::vector<int> GetFrequencies();

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
    int MakeDecision();
    std::vector<int> GetFrequencies();

private:
    Game *state_;
    MCTSNodeCS *root_;
};

struct RootParallelInput
{
    RootParallelInput(Game *b,
                      MCTSNode *root,
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
    MCTSNode *root() { return root_; }
    TimeControlStrategy *time_controller() { return time_controller_; }
    SelectionStrategy *selection_strategy() { return selection_strategy_; }
    SimulationStrategy *simulation_strategy() { return simulation_strategy_; }

private:
    Game *b_;
    MCTSNode *root_;
    TimeControlStrategy *time_controller_;
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

struct RootParallelInputMutex
{
    RootParallelInputMutex(Game *b,
                           MCTSMutexNode *root,
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
    MCTSMutexNode *root() { return root_; }
    TimeControlStrategy *time_controller() { return time_controller_; }
    SelectionStrategy *selection_strategy() { return selection_strategy_; }
    SimulationStrategy *simulation_strategy() { return simulation_strategy_; }

private:
    Game *b_;
    MCTSMutexNode *root_;
    TimeControlStrategy *time_controller_;
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MCTSMultiTree : public MCTSTree_
{
public:
    MCTSMultiTree(Game *, int);
    ~MCTSMultiTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision();
    std::vector<int> GetFrequencies();

private:
    static void *LaunchSearchThread(void *);

    Game *state_;
    int num_threads_;
    MCTSNode **roots_;
    pthread_t *threads_;
};

class MCTSParallelTree : public MCTSTree_
{
public:
    MCTSParallelTree(Game *, int);
    ~MCTSParallelTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *,
                SimulationStrategy *,
                TimeControlStrategy *);
    int MakeDecision();
    std::vector<int> GetFrequencies();

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

    int MakeDecision();
    std::vector<int> GetFrequencies();

private:
    Game *state_;
    RaveNode *root_;
};
