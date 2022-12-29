#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>
#include <map>

#include "utils.h"
#include "strategies/simulation.h"
#include "strategies/selection.h"
#include "strategies/timecontrol.h"

struct SearchParam
{
    SearchParam(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
        : state_(state), selection_strategy_{selection_strategy}, simulation_strategy_{simulation_strategy} {}

    Game *state() { return state_; }
    SelectionStrategy *selection_strategy() { return selection_strategy_; }
    SimulationStrategy *simulation_strategy() { return simulation_strategy_; }

private:
    Game *state_;
    SelectionStrategy *selection_strategy_;
    SimulationStrategy *simulation_strategy_;
};

class MCTSNode_
{
public:
    virtual ~MCTSNode_(){};
    virtual float Q() = 0;
    virtual int N() = 0;
    virtual void SetQ(float) = 0;
    virtual void SetN(int) = 0;
    virtual void UpdateResult(float) = 0;
    virtual bool IsExpanded() = 0;
    virtual void SetExpanded() = 0;
    virtual void SetNotExpanded() = 0;
    virtual const std::vector<MCTSNode_ *> *GetChildren() = 0;
    virtual void AppendChild(MCTSNode_ *) = 0;
    virtual std::vector<int> GetChildrenN() = 0;
    virtual std::vector<float> GetChildrenQ() = 0;
    virtual std::vector<std::pair<int, float>> GetChildrenQN() = 0;
    virtual void SearchOnce(SearchParam *) = 0;
    virtual Game *GetNextState(Game *state, int action_index) = 0;
};

class MCTSNodeImpl_ : public MCTSNode_
{
public:
    MCTSNodeImpl_();
    virtual ~MCTSNodeImpl_();
    float Q();
    int N();
    void SetQ(float);
    void SetN(int);
    void UpdateResult(float);
    bool IsExpanded();
    void SetExpanded();
    void SetNotExpanded();
    const std::vector<MCTSNode_ *> *GetChildren();
    void AppendChild(MCTSNode_ *);
    std::vector<int> GetChildrenN();
    std::vector<float> GetChildrenQ();
    std::vector<std::pair<int, float>> GetChildrenQN();

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
    void SearchOnce(SearchParam *);
    Game *GetNextState(Game *state, int action_index);

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
    void SearchOnce(SearchParam *);
    Game *GetNextState(Game *state, int action_index);

private:
    ActionList *actions_;
};

class MCTSMutexNode : public MCTSNodeImpl_
{
public:
    MCTSMutexNode();

    ~MCTSMutexNode();

    void Expansion(Game *);

    void SearchOnce(SearchParam *);

    void Lock();
    void Release();

    int GetVirtualN();

    void SetVirtualN(int virtual_N);
    Game *GetNextState(Game *state, int action_index);

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
    void SearchOnce(SearchParam *);

    float rave_Q(int);
    float rave_N(int);
    Game *GetNextState(Game *state, int action_index);

private:
    ActionList *actions_;
    std::map<int, std::pair<float, float>> rave_QN_;
};

class MCTSPolicyNode : public MCTSNodeImpl_
{
public:
    MCTSPolicyNode();

    ~MCTSPolicyNode();

    const std::vector<float> *GetPolicy() const;

    void Expansion(Game *state);
    void SearchOnce(SearchParam *);
    Game *GetNextState(Game *state, int action_index);

private:
    ActionList *actions_;
    std::vector<float> policy_;
};
