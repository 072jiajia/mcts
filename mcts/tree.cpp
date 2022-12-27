#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <iomanip>

#include "tree.h"

MCTSTree::MCTSTree(Game *state) : state_(state->Clone())
{
    root_ = new MCTSNode();
}

MCTSTree::~MCTSTree()
{
    delete state_;
    delete root_;
}

float MCTSTree::GetTotalSimulationCount()
{
    return root_->N();
}

void MCTSTree::Search(SelectionStrategy *selection_strategy,
                      SimulationStrategy *simulation_strategy,
                      TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        Game *cloned_state = state_->Clone();
        SearchStateParam temp{cloned_state, selection_strategy, simulation_strategy};
        root_->SearchOnce(&temp);
        delete cloned_state;
    }
}

int MCTSTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

MCTSTreeCS::MCTSTreeCS(Game *state) : state_(state->Clone())
{
    root_ = new MCTSNodeCS(state_);
}

MCTSTreeCS::~MCTSTreeCS()
{
    delete state_;
    delete root_;
}

float MCTSTreeCS::GetTotalSimulationCount()
{
    return root_->N();
}

void MCTSTreeCS::Search(SelectionStrategy *selection_strategy,
                        SimulationStrategy *simulation_strategy,
                        TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        SearchParam temp{selection_strategy, simulation_strategy};
        root_->SearchOnce(&temp);
    }
}

int MCTSTreeCS::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> MCTSTree::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<int> MCTSTreeCS::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<float> MCTSTree::GetValues()
{
    return root_->GetChildrenQ();
}

std::vector<float> MCTSTreeCS::GetValues()
{
    return root_->GetChildrenQ();
}

RaveTree::RaveTree(Game *state) : state_(state->Clone())
{
    root_ = new RaveNode();
}

RaveTree::~RaveTree()
{
    delete state_;
    delete root_;
}

float RaveTree::GetTotalSimulationCount()
{
    return root_->N();
}

void RaveTree::Search(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        Game *cloned_state = state_->Clone();
        SearchStateParam temp{cloned_state, selection_strategy, simulation_strategy};
        root_->SearchOnce(&temp);
        delete cloned_state;
    }
}

int RaveTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> RaveTree::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<float> RaveTree::GetValues()
{
    return root_->GetChildrenQ();
}
