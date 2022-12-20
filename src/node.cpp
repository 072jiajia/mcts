#include "node.h"

MCTSNodeImpl_::MCTSNodeImpl_() : Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

MCTSNodeImpl_::~MCTSNodeImpl_()
{
    for (uint i = 0; i < children_.size(); i++)
    {
        delete children_[i];
    }
}

float MCTSNodeImpl_::GetValueForCurrentPlayer() { return Q_; }

float MCTSNodeImpl_::GetTotalSimulationCount() { return N_; }

float MCTSNodeImpl_::Q() { return Q_; }
float MCTSNodeImpl_::N() { return N_; }
bool MCTSNodeImpl_::IsExpanded() { return expanded_; }

std::vector<MCTSNode_ *> MCTSNodeImpl_::GetChildren() const { return children_; }

int MCTSNodeImpl_::ChooseMoveWithMostFrequency()
{
    float best_value = -1e20;
    int best_move = -1;
    for (uint i = 0; i < children_.size(); i++)
    {
        float value = children_[i]->N();

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}

std::vector<int> MCTSNodeImpl_::GetFrequencies()
{
    std::vector<int> output;
    for (uint i = 0; i < children_.size(); i++)
    {
        output.push_back(children_[i]->N());
    }
    return output;
}

float MCTSNodeImpl_::EvaluateGameOverNode(Game *state)
{
    if (this->N_ == 0)
        this->Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
    this->N_++;
    return -this->Q_;
}

Game *MCTSNodeCS::GetCurrentState() { return state_; }

MCTSNodeCS::MCTSNodeCS(Game *s) : MCTSNodeImpl_(), state_(s->Clone()) {}

MCTSNodeCS::~MCTSNodeCS() { delete state_; }

void MCTSNodeCS::Expansion()
{
    ActionList *movable_actions = state_->GetLegalMoves();

    for (uint i = 0; i < movable_actions->GetSize(); i++)
    {
        MCTSNodeCS *new_node = new MCTSNodeCS(state_);
        new_node->state_->DoAction(movable_actions->Get(i));
        this->children_.push_back(new_node);
    }

    this->expanded_ = true;

    delete movable_actions;
}

float MCTSNodeCS::SearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state_->IsGameOver())
        return EvaluateGameOverNode(state_);
    if (this->N_ == 0)
    {
        this->Q_ = simulation_strategy->SimulationOnce(state_);
        this->N_ = 1;
        return -this->Q_;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->expanded_)
        this->Expansion();

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    MCTSNodeCS *child = (MCTSNodeCS *)(this->children_[action_index]);
    float q = child->SearchOnce(selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
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
        this->children_.push_back(new MCTSNode());
    }
    this->expanded_ = true;
}

float MCTSNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
        return EvaluateGameOverNode(state);
    if (this->N_ == 0)
    {
        this->Q_ = simulation_strategy->SimulationOnce(state);
        this->N_ = 1;
        return -this->Q_;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->expanded_)
        this->Expansion(state);

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    state->DoAction(actions_->Get(action_index));
    MCTSNode *child = (MCTSNode *)(this->children_[action_index]);
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}
