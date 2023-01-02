#include "../node.h"
#include "../utils.h"
#include "search.h"

MCTSSearch::MCTSSearch(SelectionStrategy *selection_strategy,
                       SimulationStrategy *simulation_strategy)
    : selection_strategy_(selection_strategy),
      simulation_strategy_(simulation_strategy) {}

MCTSNode_ *MCTSSearch::CreateNode(Game *state) const
{
    return new MCTSNode();
}

std::vector<float> *MCTSSearch::SearchOnce(MCTSNode_ *node, Game *input_state) const
{
    std::vector<MCTSNode *> traversed_nodes;
    MCTSNode *current_node = (MCTSNode *)node;

    Game *state = input_state->Clone();

    float value;

    while (true)
    {
        traversed_nodes.push_back(current_node);
        if (state->IsGameOver())
        {
            value = EvaluateResult(state, state->GetPlayerThisTurn());
            break;
        }
        if (current_node->N() == 0)
        {
            value = simulation_strategy_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy_->Select(current_node);
        Action *action = current_node->GetActions()->Get(action_index);
        state->DoAction(action);
        current_node = (MCTSNode *)(current_node->GetChildren()->at(action_index));
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
    delete state;
}

MCTSCopyStateSearch::MCTSCopyStateSearch(SelectionStrategy *selection_strategy,
                                         SimulationStrategy *simulation_strategy)
    : selection_strategy_(selection_strategy),
      simulation_strategy_(simulation_strategy) {}

MCTSNode_ *MCTSCopyStateSearch::CreateNode(Game *state) const
{
    return new MCTSNodeCS(state);
}

std::vector<float> *MCTSCopyStateSearch::SearchOnce(MCTSNode_ *node, Game *dummy) const
{
    std::vector<MCTSNodeCS *> traversed_nodes;
    MCTSNodeCS *current_node = (MCTSNodeCS *)node;

    float value;
    while (true)
    {
        Game *state = current_node->GetState();
        traversed_nodes.push_back(current_node);
        if (state->IsGameOver())
        {
            value = EvaluateResult(state, state->GetPlayerThisTurn());
            break;
        }
        if (current_node->N() == 0)
        {
            value = simulation_strategy_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion();

        int action_index = selection_strategy_->Select(current_node);
        current_node = (MCTSNodeCS *)(current_node->GetChildren()->at(action_index));
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
}

MutexSearch::MutexSearch(SelectionStrategy *selection_strategy,
                         SimulationStrategy *simulation_strategy)
    : selection_strategy_(selection_strategy),
      simulation_strategy_(simulation_strategy) {}

MCTSNode_ *MutexSearch::CreateNode(Game *state) const
{
    return new MCTSMutexNode();
}

std::vector<float> *MutexSearch::SearchOnce(MCTSNode_ *node, Game *input_state) const
{
    Game *state = input_state->Clone();
    std::vector<MCTSMutexNode *> traversed_nodes;
    MCTSMutexNode *current_node = (MCTSMutexNode *)node;

    float value;

    while (true)
    {
        traversed_nodes.push_back(current_node);
        if (state->IsGameOver())
        {
            value = EvaluateResult(state, state->GetPlayerThisTurn());
            break;
        }
        if (current_node->N() == 0)
        {
            value = simulation_strategy_->SimulationOnce(state);
            break;
        }

        current_node->Lock();
        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy_->Select(current_node);
        MCTSMutexNode *next_node = (MCTSMutexNode *)(current_node->GetChildren()->at(action_index));

        next_node->SetVirtualN(next_node->GetVirtualN() + 1);

        current_node->Release();

        Action *action = current_node->GetActions()->Get(action_index);
        state->DoAction(action);

        current_node = next_node;
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        MCTSMutexNode *node = traversed_nodes[i];
        node->Lock();
        node->SetVirtualN(node->GetVirtualN() - 1);
        node->UpdateResult(value);
        node->Release();

        value = -value;
    }
    delete state;
}

RaveSearch::RaveSearch(SelectionStrategy *selection_strategy,
                       SimulationStrategy *simulation_strategy)
    : selection_strategy_(selection_strategy),
      simulation_strategy_(simulation_strategy) {}

MCTSNode_ *RaveSearch::CreateNode(Game *state) const
{
    return new RaveNode();
}

std::vector<float> *RaveSearch::SearchOnce(MCTSNode_ *node, Game *input_state) const
{
    Game *state = input_state->Clone();

    std::vector<RaveNode *> traversed_nodes;
    RaveNode *current_node = (RaveNode *)node;

    std::vector<Action *> action_history;

    float value;

    while (true)
    {
        traversed_nodes.push_back(current_node);
        if (state->IsGameOver())
        {
            value = EvaluateResult(state, state->GetPlayerThisTurn());
            break;
        }
        if (current_node->N() == 0)
        {
            value = simulation_strategy_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy_->Select(current_node);
        Action *selected_action = current_node->GetAction()->Get(action_index);
        action_history.push_back(selected_action);

        state->DoAction(selected_action);
        current_node = (RaveNode *)(current_node->GetChildren()->at(action_index));
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);

        for (int j = i; j < action_history.size(); j += 2)
        {
            RaveNode *node = traversed_nodes[j];
            std::map<int, std::pair<float, float>> &rave_QN = *(node->GetRaveQN());

            int action_enc = action_history[i]->encoding();
            if (rave_QN.find(action_enc) == rave_QN.end())
            {
                rave_QN[action_enc] = {0., 0.};
            }
            std::pair<float, float> &QN = rave_QN[action_enc];
            QN.first += (value - QN.first) / ++QN.second;
        }

        value = -value;
    }
    delete state;
}

PolicySearch::PolicySearch(SelectionStrategy *selection_strategy,
                           SimulationStrategy *simulation_strategy,
                           PolicyStrategy *policy_strategy)
    : selection_strategy_(selection_strategy),
      simulation_strategy_(simulation_strategy),
      policy_strategy_(policy_strategy) {}

MCTSNode_ *PolicySearch::CreateNode(Game *state) const
{
    return new MCTSPolicyNode();
}

std::vector<float> *PolicySearch::SearchOnce(MCTSNode_ *node, Game *input_state) const
{
    Game *state = input_state->Clone();

    std::vector<MCTSPolicyNode *> traversed_nodes;
    MCTSPolicyNode *current_node = (MCTSPolicyNode *)node;

    float value;

    while (true)
    {
        traversed_nodes.push_back(current_node);
        if (state->IsGameOver())
        {
            value = EvaluateResult(state, state->GetPlayerThisTurn());
            break;
        }
        if (current_node->N() == 0)
        {
            value = simulation_strategy_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
        {
            current_node->Expansion(state);
            current_node->SetPolicy(policy_strategy_->Estimate(current_node, state, current_node->GetActions()));
        }

        int action_index = selection_strategy_->Select(current_node);

        Action *action = current_node->GetActions()->Get(action_index);
        state->DoAction(action);

        current_node = (MCTSPolicyNode *)(current_node->GetChildren()->at(action_index));
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
    delete state;
}