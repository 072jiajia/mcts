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

MCTSTree::MCTSTree(MCTSNode_ *root, Game *state) : root_(root), state_(state->Clone()) {}

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

MCTSTreeCS::MCTSTreeCS(MCTSNode_ *root, Game *state) : root_(root), state_(state->Clone()) {}

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
