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
#include "timecontrol.h"

class MCTSNode_
{
public:
    virtual ~MCTSNode_(){};
    virtual float GetValueForCurrentPlayer() = 0;
    virtual float GetTotalSimulationCount() = 0;
    virtual float Q() = 0;
    virtual float N() = 0;
    virtual bool IsExpanded() = 0;
    virtual const std::vector<MCTSNode_ *> *GetChildren() const = 0;

    virtual int ChooseMoveWithMostFrequency() = 0;
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
    const std::vector<MCTSNode_ *> *GetChildren() const;
    int ChooseMoveWithMostFrequency();

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

class MCTSTree_
{
public:
    virtual ~MCTSTree_(){};
    virtual float GetTotalSimulationCount() = 0;
    virtual void Search(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, TimeControlStrategy *time_controller) = 0;
    virtual int MakeDecision() = 0;
    virtual std::vector<int> GetFrequencies() = 0;
};

class MCTSTree : public MCTSTree_
{
public:
    MCTSTree(Game *state);
    ~MCTSTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *selection_strategy,
                SimulationStrategy *simulation_strategy,
                TimeControlStrategy *time_controller);
    int MakeDecision();
    std::vector<int> GetFrequencies();

private:
    Game *state_;
    MCTSNode *root_;
};

class MCTSTreeCS : public MCTSTree_
{
public:
    MCTSTreeCS(Game *state);
    ~MCTSTreeCS();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *selection_strategy,
                SimulationStrategy *simulation_strategy,
                TimeControlStrategy *time_controller);
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

class MCTSMultiTree : public MCTSTree_
{
public:
    MCTSMultiTree(Game *state, int num_threads);
    ~MCTSMultiTree();
    float GetTotalSimulationCount();
    void Search(SelectionStrategy *selection_strategy,
                SimulationStrategy *simulation_strategy,
                TimeControlStrategy *time_controller);
    int MakeDecision();
    std::vector<int> GetFrequencies();

private:
    static void *LaunchSearchThread(void *args_void);

    Game *state_;
    int num_threads_;
    MCTSNode **roots_;
    pthread_t *threads_;
};
