#include "policy.h"
#include "../node.h"

DefaultPolicy::DefaultPolicy() {}

std::vector<float> *DefaultPolicy::Estimate(const MCTSNode_ *node, const Game *state, const ActionList *action_list) const
{
    return new std::vector<float>(action_list->GetSize(), 1. / action_list->GetSize());
}
