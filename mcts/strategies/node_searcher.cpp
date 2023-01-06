#include "../node.h"
#include "../utils.h"
#include "node_searcher.h"

MCTSNodeSearcher::MCTSNodeSearcher(SelectionStrategy *selection_strategy,
                                   SimulationStrategy *simulation_strategy)
    : selection_(selection_strategy),
      simulation_(simulation_strategy)
{
    if (!selection_)
        selection_ = new UCBHighest(1.4);

    if (!simulation_)
        simulation_ = new SimulationDefaultStrategy();
}

MCTSNodeSearcher::~MCTSNodeSearcher()
{
    delete selection_;
    delete simulation_;
}

MCTSNode_ *MCTSNodeSearcher::CreateNode(const Game *state) const
{
    return new MCTSNode();
}

bool MCTSNodeSearcher::DoesSupportTreeParallel() const
{
    return false;
}

void MCTSNodeSearcher::SearchOnce(MCTSNode_ *node, const Game *input_state) const
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
            value = simulation_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_->Select(current_node);
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

MCTSNodeCSSearcher::MCTSNodeCSSearcher(SelectionStrategy *selection_strategy,
                                       SimulationStrategy *simulation_strategy)
    : selection_(selection_strategy),
      simulation_(simulation_strategy)
{
    if (!selection_)
        selection_ = new UCBHighest(1.4);

    if (!simulation_)
        simulation_ = new SimulationDefaultStrategy();
}

MCTSNodeCSSearcher::~MCTSNodeCSSearcher()
{
    delete selection_;
    delete simulation_;
}

MCTSNode_ *MCTSNodeCSSearcher::CreateNode(const Game *state) const
{
    return new MCTSNodeCS(state);
}

bool MCTSNodeCSSearcher::DoesSupportTreeParallel() const
{
    return false;
}

void MCTSNodeCSSearcher::SearchOnce(MCTSNode_ *node, const Game *dummy) const
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
            value = simulation_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion();

        int action_index = selection_->Select(current_node);
        current_node = (MCTSNodeCS *)(current_node->GetChildren()->at(action_index));
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
}

MutexNodeSearcher::MutexNodeSearcher(SelectionStrategy *selection_strategy,
                                     SimulationStrategy *simulation_strategy)
    : selection_(selection_strategy),
      simulation_(simulation_strategy)
{
    if (!selection_)
        selection_ = new UCBHighestVirtualLoss(1.4);

    if (!simulation_)
        simulation_ = new SimulationDefaultStrategy();
}

MutexNodeSearcher::~MutexNodeSearcher()
{
    delete selection_;
    delete simulation_;
}

MCTSNode_ *MutexNodeSearcher::CreateNode(const Game *state) const
{
    return new MCTSMutexNode();
}

bool MutexNodeSearcher::DoesSupportTreeParallel() const
{
    return true;
}

void MutexNodeSearcher::SearchOnce(MCTSNode_ *node, const Game *input_state) const
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
            value = simulation_->SimulationOnce(state);
            break;
        }

        current_node->Lock();
        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_->Select(current_node);
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

RaveNodeSearcher::RaveNodeSearcher(SelectionStrategy *selection_strategy,
                                   SimulationStrategy *simulation_strategy)
    : selection_(selection_strategy),
      simulation_(simulation_strategy)
{
    if (!selection_)
        selection_ = new RaveUCBHighest(1.4, 0.1);

    if (!simulation_)
        simulation_ = new SimulationDefaultStrategy();
}

RaveNodeSearcher::~RaveNodeSearcher()
{
    delete selection_;
    delete simulation_;
}

MCTSNode_ *RaveNodeSearcher::CreateNode(const Game *state) const
{
    return new RaveNode();
}

bool RaveNodeSearcher::DoesSupportTreeParallel() const
{
    return false;
}

void RaveNodeSearcher::SearchOnce(MCTSNode_ *node, const Game *input_state) const
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
            value = simulation_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_->Select(current_node);
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

PolicyNodeSearcher::PolicyNodeSearcher(SelectionStrategy *selection_strategy,
                                       SimulationStrategy *simulation_strategy,
                                       PolicyStrategy *policy_strategy)
    : selection_(selection_strategy),
      simulation_(simulation_strategy),
      policy_(policy_strategy)
{
    if (!selection_)
        selection_ = new PUCT(1.4);

    if (!simulation_)
        simulation_ = new SimulationDefaultStrategy();

    if (!policy_)
        policy_ = new DefaultPolicy();
}

PolicyNodeSearcher::~PolicyNodeSearcher()
{
    delete selection_;
    delete simulation_;
    delete policy_;
}

MCTSNode_ *PolicyNodeSearcher::CreateNode(const Game *state) const
{
    return new MCTSPolicyNode();
}

bool PolicyNodeSearcher::DoesSupportTreeParallel() const
{
    return false;
}

void PolicyNodeSearcher::SearchOnce(MCTSNode_ *node, const Game *input_state) const
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
            value = simulation_->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
        {
            current_node->Expansion(state);
            current_node->SetPolicy(policy_->Estimate(current_node, state, current_node->GetActions()));
        }

        int action_index = selection_->Select(current_node);

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