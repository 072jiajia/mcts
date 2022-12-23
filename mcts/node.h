#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>
#include <map>

#include "utils.h"
#include "selection.h"
#include "simulation.h"
#include "timecontrol.h"

class MCTSNode_
{
public:
    virtual ~MCTSNode_(){};
    virtual float Q() = 0;
    virtual float N() = 0;
    virtual void SetQ(float) = 0;
    virtual void SetN(float) = 0;
    virtual bool IsExpanded() = 0;
    virtual void SetExpanded() = 0;
    virtual void SetNotExpanded() = 0;
    virtual std::vector<MCTSNode_ *> *GetChildren() = 0;
};

class MCTSNodeImpl_ : public MCTSNode_
{
public:
    MCTSNodeImpl_();
    virtual ~MCTSNodeImpl_();
    float Q();
    float N();
    void SetQ(float);
    void SetN(float);
    bool IsExpanded();
    void SetExpanded();
    void SetNotExpanded();
    std::vector<MCTSNode_ *> *GetChildren();

protected:
    float EvaluateGameOverNode(Game *);

private:
    std::vector<MCTSNode_ *> children_;
    float Q_;
    float N_;
    bool expanded_;
};

class MCTSNodeCS : public MCTSNodeImpl_
{
public:
    MCTSNodeCS(Game *s);
    ~MCTSNodeCS();
    void Expansion();
    float SearchOnce(SelectionStrategy *, SimulationStrategy *);

private:
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
    ~MCTSNode();
    void Expansion(Game *);
    float SearchOnce(Game *, SelectionStrategy *, SimulationStrategy *);

private:
    ActionList *actions_;
};

class MCTSMutexNode : public MCTSNodeImpl_
{
public:
    MCTSMutexNode();

    ~MCTSMutexNode();

    void Expansion(Game *);

    float SearchOnce(Game *, SelectionStrategy *, SimulationStrategy *);

    void Lock();
    void Release();

    float GetVirtualN();

    void SetVirtualN(float virtual_N);

private:
    ActionList *actions_;
    sem_t lock;
    int virtual_N_;
};

class RaveNode : public MCTSNodeImpl_
{
    /*
     * Implementation of this paper (https://doi.org/10.1016/j.artint.2011.03.007)
     */
public:
    RaveNode();

    ~RaveNode();

    void Expansion(Game *state);
    float SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, std::vector<int> &self_action, std::vector<int> &oppo_action);

    float rave_Q(int);
    float rave_N(int);

private:
    ActionList *actions_;
    std::map<int, std::pair<float, float>> rave_QN_;
};
