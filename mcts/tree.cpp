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
        SearchParam temp{state_, selection_strategy, simulation_strategy};
        root_->SearchOnce(&temp);
    }
}

int MCTSTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> MCTSTree::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<float> MCTSTree::GetValues()
{
    return root_->GetChildrenQ();
}
