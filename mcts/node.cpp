#include "node.h"

MCTSNodeImpl_::MCTSNodeImpl_() : children_(), Q_(rand() * 0.001 / RAND_MAX), N_(0), expanded_(false) {}

MCTSNodeImpl_::~MCTSNodeImpl_()
{
    for (uint i = 0; i < children_.size(); i++)
    {
        delete children_[i];
    }
}

float MCTSNodeImpl_::Q() { return Q_; }
int MCTSNodeImpl_::N() { return N_; }
void MCTSNodeImpl_::SetQ(float Q) { Q_ = Q; }
void MCTSNodeImpl_::SetN(int N) { N_ = N; }
bool MCTSNodeImpl_::IsExpanded() { return expanded_; }
void MCTSNodeImpl_::SetExpanded() { expanded_ = true; }
void MCTSNodeImpl_::SetNotExpanded() { expanded_ = false; }
const std::vector<MCTSNode_ *> *MCTSNodeImpl_::GetChildren() { return &children_; }
void MCTSNodeImpl_::AppendChild(MCTSNode_ *new_child) { children_.push_back(new_child); }

std::vector<int> MCTSNodeImpl_::GetChildrenN()
{
    std::vector<int> output;
    for (int i = 0; i < children_.size(); i++)
    {
        MCTSNode_ *node = children_.at(i);
        output.push_back(node->N());
    }
    return output;
}

std::vector<float> MCTSNodeImpl_::GetChildrenQ()
{
    std::vector<float> output;
    for (int i = 0; i < children_.size(); i++)
    {
        MCTSNode_ *node = children_.at(i);
        output.push_back(node->Q());
    }
    return output;
}

std::vector<std::pair<int, float>> MCTSNodeImpl_::GetChildrenQN()
{
    std::vector<std::pair<int, float>> output;
    for (int i = 0; i < children_.size(); i++)
    {
        MCTSNode_ *node = children_.at(i);
        output.push_back({node->Q(), node->N()});
    }
    return output;
}

void MCTSNodeImpl_::UpdateResult(float q)
{
    Q_ += (q - Q_) / ++N_;
}

MCTSNodeCS::MCTSNodeCS(Game *s) : MCTSNodeImpl_(), state_(s->Clone()) {}

MCTSNodeCS::~MCTSNodeCS() { delete state_; }

void MCTSNodeCS::Expansion(Game *dummy)
{
    ActionList *movable_actions = state_->GetLegalMoves();

    for (uint i = 0; i < movable_actions->GetSize(); i++)
    {
        MCTSNodeCS *new_node = new MCTSNodeCS(state_);
        new_node->state_->DoAction(movable_actions->Get(i));
        this->AppendChild(new_node);
    }

    this->SetExpanded();

    delete movable_actions;
}

Game *MCTSNodeCS::GetNextState(Game *state, int action_index)
{
    MCTSNodeCS *node = (MCTSNodeCS *)(GetChildren()->at(action_index));
    return node->state_;
}

void MCTSNodeCS::SearchOnce(SearchParam *input)
{
    Game *cloned_state = input->state()->Clone();
    SelectionStrategy *selection_strategy = input->selection_strategy();
    SimulationStrategy *simulation_strategy = input->simulation_strategy();

    std::vector<MCTSNode_ *> traversed_nodes;
    MCTSNode_ *current_node = this;

    Game *state = cloned_state;

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
            value = simulation_strategy->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy->Select(current_node);
        state = current_node->GetNextState(state, action_index);
        current_node = current_node->GetChildren()->at(action_index);
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
    delete cloned_state;
}

MCTSNode::MCTSNode() : MCTSNodeImpl_(), actions_(nullptr) {}

MCTSNode::~MCTSNode()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
}

void MCTSNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->AppendChild(new MCTSNode());
    }
    this->SetExpanded();
}

Game *MCTSNode::GetNextState(Game *state, int action_index)
{
    state->DoAction(actions_->Get(action_index));
    return state;
}

void MCTSNode::SearchOnce(SearchParam *input)
{
    Game *cloned_state = input->state()->Clone();
    SelectionStrategy *selection_strategy = input->selection_strategy();
    SimulationStrategy *simulation_strategy = input->simulation_strategy();

    std::vector<MCTSNode_ *> traversed_nodes;
    MCTSNode_ *current_node = this;

    Game *state = cloned_state;

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
            value = simulation_strategy->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy->Select(current_node);
        state = current_node->GetNextState(state, action_index);
        current_node = current_node->GetChildren()->at(action_index);
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
    delete cloned_state;
}

MCTSMutexNode::MCTSMutexNode() : MCTSNodeImpl_(), virtual_N_(0), actions_(nullptr), lock()
{
    sem_init(&lock, 0, 1);
}

MCTSMutexNode::~MCTSMutexNode()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
}

void MCTSMutexNode::Lock()
{
    sem_wait(&lock);
}

void MCTSMutexNode::Release()
{
    sem_post(&lock);
}

void MCTSMutexNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->AppendChild(new MCTSMutexNode());
    }
    this->SetExpanded();
}

int MCTSMutexNode::GetVirtualN()
{
    return virtual_N_;
}

void MCTSMutexNode::SetVirtualN(int virtual_N)
{
    virtual_N_ = virtual_N;
}

Game *MCTSMutexNode::GetNextState(Game *state, int action_index)
{
    state->DoAction(actions_->Get(action_index));
    return state;
}

void MCTSMutexNode::SearchOnce(SearchParam *input)
{
    Game *state = input->state()->Clone();
    SelectionStrategy *selection_strategy = input->selection_strategy();
    SimulationStrategy *simulation_strategy = input->simulation_strategy();

    std::vector<MCTSMutexNode *> traversed_nodes;
    MCTSMutexNode *current_node = this;

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
            value = simulation_strategy->SimulationOnce(state);
            break;
        }

        current_node->Lock();
        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy->Select(current_node);
        MCTSMutexNode *next_node = (MCTSMutexNode *)(current_node->GetChildren()->at(action_index));

        next_node->Lock();
        next_node->SetVirtualN(next_node->GetVirtualN() + 1);
        next_node->Release();

        current_node->Release();

        state->DoAction(current_node->actions_->Get(action_index));
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

RaveNode::RaveNode() : MCTSNodeImpl_(), actions_(nullptr), rave_QN_() {}

RaveNode::~RaveNode() {}

float RaveNode::rave_Q(int index)
{
    int action_index = actions_->Get(index)->encoding();

    if (rave_QN_.find(action_index) == rave_QN_.end())
        return 0.;

    return rave_QN_[action_index].first;
}

float RaveNode::rave_N(int index)
{
    int action_index = actions_->Get(index)->encoding();

    if (rave_QN_.find(action_index) == rave_QN_.end())
        return 0.;

    return rave_QN_[action_index].second;
}

void RaveNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->AppendChild(new RaveNode());
    }
    this->SetExpanded();
}

Game *RaveNode::GetNextState(Game *state, int action_index)
{
    state->DoAction(actions_->Get(action_index));
    return state;
}

void RaveNode::SearchOnce(SearchParam *input)
{
    Game *state = input->state()->Clone();
    SelectionStrategy *selection_strategy = input->selection_strategy();
    SimulationStrategy *simulation_strategy = input->simulation_strategy();

    std::vector<RaveNode *> traversed_nodes;
    RaveNode *current_node = this;

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
            value = simulation_strategy->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy->Select(current_node);
        Action *selected_action = current_node->actions_->Get(action_index);
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

            int action_enc = action_history[i]->encoding();
            if (node->rave_QN_.find(action_enc) == node->rave_QN_.end())
            {
                node->rave_QN_[action_enc] = {0., 0.};
            }
            std::pair<float, float> &QN = node->rave_QN_[action_enc];
            QN.first += (value - QN.first) / ++QN.second;
        }

        value = -value;
    }
    delete state;
}

MCTSPolicyNode::MCTSPolicyNode() : MCTSNodeImpl_(), actions_(nullptr), policy_() {}

MCTSPolicyNode::~MCTSPolicyNode()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
}

const std::vector<float> *MCTSPolicyNode::GetPolicy() const
{
    return &policy_;
}

void MCTSPolicyNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();
    policy_.resize(actions_->GetSize(), 1. / actions_->GetSize());
    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->AppendChild(new MCTSPolicyNode());
    }
    this->SetExpanded();
}

Game *MCTSPolicyNode::GetNextState(Game *state, int action_index)
{
    state->DoAction(actions_->Get(action_index));
    return state;
}

void MCTSPolicyNode::SearchOnce(SearchParam *input)
{
    Game *state = input->state()->Clone();
    SelectionStrategy *selection_strategy = input->selection_strategy();
    SimulationStrategy *simulation_strategy = input->simulation_strategy();

    std::vector<MCTSPolicyNode *> traversed_nodes;
    MCTSPolicyNode *current_node = this;

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
            value = simulation_strategy->SimulationOnce(state);
            break;
        }

        if (!current_node->IsExpanded())
            current_node->Expansion(state);

        int action_index = selection_strategy->Select(current_node);
        state->DoAction(current_node->actions_->Get(action_index));
        current_node = (MCTSPolicyNode *)(current_node->GetChildren()->at(action_index));
    }

    for (int i = traversed_nodes.size() - 1; i >= 0; i--)
    {
        traversed_nodes[i]->UpdateResult(value);
        value = -value;
    }
    delete state;
}
