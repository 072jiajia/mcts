#include "node.h"

MCTSNodeImpl_::MCTSNodeImpl_() : children_(), Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

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
std::vector<MCTSNode_ *> *MCTSNodeImpl_::GetChildren() { return &children_; }

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

float MCTSNodeImpl_::EvaluateGameOverNode(Game *state)
{
    if (this->N() == 0)
        this->SetQ(EvaluateResult(state, state->GetPlayerThisTurn()));
    this->SetN(this->N() + 1);
    return -this->Q();
}

MCTSNodeCS::MCTSNodeCS(Game *s) : MCTSNodeImpl_(), state_(s->Clone()) {}

MCTSNodeCS::~MCTSNodeCS() { delete state_; }

void MCTSNodeCS::Expansion()
{
    ActionList *movable_actions = state_->GetLegalMoves();

    for (uint i = 0; i < movable_actions->GetSize(); i++)
    {
        MCTSNodeCS *new_node = new MCTSNodeCS(state_);
        new_node->state_->DoAction(movable_actions->Get(i));
        this->GetChildren()->push_back(new_node);
    }

    this->SetExpanded();

    delete movable_actions;
}

float MCTSNodeCS::SearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state_->IsGameOver())
        return EvaluateGameOverNode(state_);
    if (this->N() == 0)
    {
        this->UpdateResult(simulation_strategy->SimulationOnce(state_));
        return -this->Q();
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->IsExpanded())
        this->Expansion();

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    MCTSNodeCS *child = (MCTSNodeCS *)(this->GetChildren()->at(action_index));
    float q = child->SearchOnce(selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->UpdateResult(q);

    return -q;
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
        this->GetChildren()->push_back(new MCTSNode());
    }
    this->SetExpanded();
}

float MCTSNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
        return EvaluateGameOverNode(state);
    if (this->N() == 0)
    {
        this->UpdateResult(simulation_strategy->SimulationOnce(state));
        return -this->Q();
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->IsExpanded())
        this->Expansion(state);

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    state->DoAction(actions_->Get(action_index));
    MCTSNode *child = (MCTSNode *)(this->GetChildren()->at(action_index));
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->UpdateResult(q);

    return -q;
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
        this->GetChildren()->push_back(new MCTSMutexNode());
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

float MCTSMutexNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
    {
        this->Lock();

        if (this->N() == 0)
            this->SetQ(EvaluateResult(state, state->GetPlayerThisTurn()));

        this->SetN(this->N() + 1);
        this->SetVirtualN(this->GetVirtualN() - 1);
        float output = -this->Q();
        this->Release();
        return output;
    }
    if (this->N() == 0)
    {
        this->Lock();
        this->UpdateResult(simulation_strategy->SimulationOnce(state));

        this->SetVirtualN(this->GetVirtualN() - 1);
        float output = -this->Q();

        this->Release();
        return output;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */

    this->Lock();
    if (!this->IsExpanded())
        this->Expansion(state);

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    state->DoAction(actions_->Get(action_index));
    MCTSMutexNode *child = (MCTSMutexNode *)(this->GetChildren()->at(action_index));

    child->Lock();
    child->SetVirtualN(child->GetVirtualN() + 1);
    child->Release();

    this->Release();

    float q = child->SearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Lock();
    this->UpdateResult(q);
    this->SetVirtualN(this->GetVirtualN() - 1);
    this->Release();

    return -q;
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
        this->GetChildren()->push_back(new RaveNode());
    }
    this->SetExpanded();
}

float RaveNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, std::vector<int> &self_action, std::vector<int> &oppo_action)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
        return EvaluateGameOverNode(state);
    if (this->N() == 0)
    {
        this->UpdateResult(simulation_strategy->SimulationOnce(state));
        return -this->Q();
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->IsExpanded())
        this->Expansion(state);

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    Action *selected_action = actions_->Get(action_index);
    state->DoAction(selected_action);
    RaveNode *child = (RaveNode *)(this->GetChildren()->at(action_index));
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy, oppo_action, self_action);

    /* Back Propagation */
    this->UpdateResult(q);

    self_action.push_back(selected_action->encoding());
    for (uint i = 0; i < self_action.size(); i++)
    {
        int action_enc = self_action[i];
        if (rave_QN_.find(action_enc) == rave_QN_.end())
        {
            rave_QN_[action_enc] = {0., 0.};
        }
        std::pair<float, float> &QN = this->rave_QN_[action_enc];
        QN.first += (q - QN.first) / ++QN.second;
    }

    return -q;
}
