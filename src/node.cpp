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
float MCTSNodeImpl_::N() { return N_; }
void MCTSNodeImpl_::SetQ(float Q) { Q_ = Q; }
void MCTSNodeImpl_::SetN(float N) { N_ = N; }
bool MCTSNodeImpl_::IsExpanded() { return expanded_; }
void MCTSNodeImpl_::SetExpanded() { expanded_ = true; }
void MCTSNodeImpl_::SetNotExpanded() { expanded_ = false; }
std::vector<MCTSNode_ *> *MCTSNodeImpl_::GetChildren() { return &children_; }

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
        this->SetQ(simulation_strategy->SimulationOnce(state_));
        this->SetN(1);
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
    float newQ = this->Q() + (q - this->Q()) / (this->N() + 1);
    this->SetQ(newQ);
    this->SetN(this->N() + 1);
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
        this->SetQ(simulation_strategy->SimulationOnce(state));
        this->SetN(1);
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
    float newQ = this->Q() + (q - this->Q()) / (this->N() + 1);
    this->SetQ(newQ);
    this->SetN(this->N() + 1);
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

sem_t *MCTSMutexNode::GetLock()
{
    return &lock;
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

float MCTSMutexNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
    {
        sem_wait(&lock);

        if (this->N() == 0)
            this->SetQ(EvaluateResult(state, state->GetPlayerThisTurn()));

        this->SetN(this->N() + 1);
        this->virtual_N_--;
        float output = -this->Q();
        sem_post(&lock);
        return output;
    }
    if (this->N() == 0)
    {
        sem_wait(&lock);
        this->SetQ(simulation_strategy->SimulationOnce(state));
        this->SetN(1);

        this->virtual_N_--;
        float output = -this->Q();

        sem_post(&lock);
        return output;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */

    sem_wait(&lock);
    if (!this->IsExpanded())
        this->Expansion(state);
    sem_post(&lock);

    /* Recursively select the child nodes */
    sem_wait(&lock);
    int action_index = selection_strategy->Select(this);
    sem_post(&lock);
    state->DoAction(actions_->Get(action_index));
    MCTSMutexNode *child = (MCTSMutexNode *)(this->GetChildren()->at(action_index));
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    sem_wait(&lock);
    float newQ = this->Q() + (q - this->Q()) / (this->N() + 1);
    this->SetQ(newQ);
    this->SetN(this->N() + 1);
    this->virtual_N_--;
    sem_post(&lock);

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
        this->SetQ(simulation_strategy->SimulationOnce(state));
        this->SetN(1);
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
    float newQ = this->Q() + (q - this->Q()) / (this->N() + 1);
    this->SetQ(newQ);
    this->SetN(this->N() + 1);

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
