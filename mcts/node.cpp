#include "node.h"

MCTSNodeImpl_::MCTSNodeImpl_() : children_(), Q_(rand() * 0.001 / RAND_MAX), N_(0), expanded_(false) {}

MCTSNodeImpl_::~MCTSNodeImpl_()
{
    for (uint i = 0; i < children_.size(); i++)
    {
        if (!children_[i])
        {
            delete children_[i];
        }
    }
}

float MCTSNodeImpl_::Q() const { return Q_; }
int MCTSNodeImpl_::N() const { return N_; }
void MCTSNodeImpl_::SetQ(float Q) { Q_ = Q; }
void MCTSNodeImpl_::SetN(int N) { N_ = N; }
bool MCTSNodeImpl_::IsExpanded() const { return expanded_; }
void MCTSNodeImpl_::SetExpanded() { expanded_ = true; }
void MCTSNodeImpl_::SetNotExpanded() { expanded_ = false; }
const std::vector<MCTSNode_ *> *MCTSNodeImpl_::GetChildren() { return &children_; }
void MCTSNodeImpl_::AppendChild(MCTSNode_ *new_child) { children_.push_back(new_child); }

std::vector<int> MCTSNodeImpl_::GetChildrenN() const
{
    std::vector<int> output;
    for (int i = 0; i < children_.size(); i++)
    {
        MCTSNode_ *node = children_.at(i);
        output.push_back(node->N());
    }
    return output;
}

std::vector<float> MCTSNodeImpl_::GetChildrenQ() const
{
    std::vector<float> output;
    for (int i = 0; i < children_.size(); i++)
    {
        MCTSNode_ *node = children_.at(i);
        output.push_back(node->Q());
    }
    return output;
}

std::vector<std::pair<int, float>> MCTSNodeImpl_::GetChildrenQN() const
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

MCTSNode_ *MCTSNodeImpl_::PopChild(int index)
{
    MCTSNode_ *output = children_[index];
    children_[index] = nullptr;
    return output;
}

MCTSNodeCS::MCTSNodeCS(const Game *s) : MCTSNodeImpl_(), state_(s->Clone()) {}

MCTSNodeCS::~MCTSNodeCS() { delete state_; }

void MCTSNodeCS::Expansion()
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

void MCTSNodeCS::SearchOnce(const Game *state, NodeSearcher_ *search_strategy)
{
    search_strategy->SearchOnce(this, state);
}

Game *MCTSNodeCS::GetState() { return state_; }

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

void MCTSNode::SearchOnce(const Game *state, NodeSearcher_ *search_strategy)
{
    search_strategy->SearchOnce(this, state);
}

const ActionList *MCTSNode::GetActions() { return actions_; }

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

void MCTSMutexNode::SearchOnce(const Game *state, NodeSearcher_ *search_strategy)
{
    search_strategy->SearchOnce(this, state);
}

const ActionList *MCTSMutexNode::GetActions() { return actions_; }

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

void RaveNode::SearchOnce(const Game *state, NodeSearcher_ *search_strategy)
{
    search_strategy->SearchOnce(this, state);
}

ActionList *RaveNode::GetAction() { return actions_; }

std::map<int, std::pair<float, float>> *RaveNode::GetRaveQN() { return &rave_QN_; }

MCTSPolicyNode::MCTSPolicyNode() : MCTSNodeImpl_(), actions_(nullptr), policy_(nullptr) {}

MCTSPolicyNode::~MCTSPolicyNode()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
    if (policy_ != nullptr)
    {
        delete policy_;
    }
}

const std::vector<float> *MCTSPolicyNode::GetPolicy() const
{
    return policy_;
}

void MCTSPolicyNode::SetPolicy(std::vector<float> *policy)
{
    policy_ = policy;
}

void MCTSPolicyNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();
    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->AppendChild(new MCTSPolicyNode());
    }
    this->SetExpanded();
}

void MCTSPolicyNode::SearchOnce(const Game *state, NodeSearcher_ *search_strategy)
{
    search_strategy->SearchOnce(this, state);
}

const ActionList *MCTSPolicyNode::GetActions() { return actions_; }
