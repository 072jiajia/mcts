#include "node.h"

float MCTSNodeBase::GetValueForCurrentPlayer() { return Q_; }

float MCTSNodeBase::GetTotalSimulationCount() { return N_; }

float MCTSNodeBase::Q() { return Q_; }
float MCTSNodeBase::N() { return N_; }
bool MCTSNodeBase::IsExpanded() { return expanded_; }

std::vector<MCTSNodeBase *> MCTSNodeBase::GetChildren() const { return children_; }

MCTSNodeBase::MCTSNodeBase() : Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

MCTSNodeBase::~MCTSNodeBase()
{
    for (uint i = 0; i < children_.size(); i++)
    {
        delete children_[i];
    }
}

int MCTSNodeBase::ChooseMoveWithMostFrequency()
{
    float best_value = -1e20;
    int best_move = -1;
    for (uint i = 0; i < children_.size(); i++)
    {
        float value = children_[i]->N_;

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}

float MCTSNodeBase::EvaluateGameOverNode(Game *state)
{
    if (this->N_ == 0)
        this->Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
    this->N_++;
    return -this->Q_;
}

Game *MCTSNode::GetCurrentState() { return state_; }

MCTSNode::MCTSNode(Game *s) : MCTSNodeBase(), state_(s->Clone()) {}

MCTSNode::~MCTSNode() { delete state_; }

void MCTSNode::Expansion()
{
    ActionList *movable_actions = state_->GetLegalMoves();

    for (uint i = 0; i < movable_actions->GetSize(); i++)
    {
        MCTSNode *new_node = new MCTSNode(state_);
        new_node->state_->DoAction(movable_actions->Get(i));
        this->children_.push_back(new_node);
    }

    this->expanded_ = true;

    delete movable_actions;
}

float MCTSNode::DoMonteCarloTreeSearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
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
    MCTSNode *child = (MCTSNode *)(this->children_[action_index]);
    float q = child->DoMonteCarloTreeSearchOnce(selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}

MCTSNodeV2::MCTSNodeV2() : MCTSNodeBase(), actions_(nullptr) {}

MCTSNodeV2::~MCTSNodeV2()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
}

void MCTSNodeV2::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->children_.push_back(new MCTSNodeV2());
    }
    this->expanded_ = true;
}

float MCTSNodeV2::DoMonteCarloTreeSearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
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
    MCTSNodeV2 *child = (MCTSNodeV2 *)(this->children_[action_index]);
    float q = child->DoMonteCarloTreeSearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}
