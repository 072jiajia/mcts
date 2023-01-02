#include "policy.h"
#include "../node.h"

DefaultPolicy::DefaultPolicy() {}

std::vector<float> *DefaultPolicy::Estimate(MCTSNode_ *node, Game *state, ActionList *action_list) const
{
    return new std::vector<float>(action_list->GetSize(), 1. / action_list->GetSize());
}
