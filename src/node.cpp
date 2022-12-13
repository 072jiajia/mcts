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

Game *MCTSNode::GetCurrentState() { return state_; }

MCTSNode::MCTSNode(Game *s) : MCTSNodeBase(), state_(s->Clone()) {}

MCTSNode::~MCTSNode() { delete state_; }

void MCTSNode::Expansion()
{
    auto movable_actions = state_->GetLegalMoves();

    for (uint i = 0; i < movable_actions.size(); i++)
    {
        MCTSNode *new_node = new MCTSNode(state_);
        new_node->state_->DoAction(movable_actions[i]);
        this->children_.push_back(new_node);
    }

    this->expanded_ = true;
}

float MCTSNode::DoMonteCarloTreeSearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    float q;
    if (state_->IsGameOver())
    {
        if (this->N_ == 0)
            this->Q_ = EvaluateResult(state_, state_->GetPlayerThisTurn());
        this->N_++;
        return -this->Q_;
    }
    else if (this->N_ == 0)
    {
        q = simulation_strategy->SimulationOnce(state_);
    }
    else
    {
        if (!this->expanded_)
            this->Expansion();
        q = ((MCTSNode *)(this->children_[selection_strategy->Select(this)]))->DoMonteCarloTreeSearchOnce(selection_strategy, simulation_strategy);
    }
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}

MCTSNodeV2::MCTSNodeV2() : MCTSNodeBase() {}

MCTSNodeV2::~MCTSNodeV2() {}

void MCTSNodeV2::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_.size(); i++)
    {
        this->children_.push_back(new MCTSNodeV2());
    }
    this->expanded_ = true;
}

float MCTSNodeV2::DoMonteCarloTreeSearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    float q;
    if (state->IsGameOver())
    {
        if (this->N_ == 0)
            this->Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
        this->N_++;
        return -this->Q_;
    }
    else if (this->N_ == 0)
    {
        q = simulation_strategy->SimulationOnce(state);
    }
    else
    {
        if (!this->expanded_)
            this->Expansion(state);

        int action_index = selection_strategy->Select(this);
        state->DoAction(actions_[action_index]);
        q = ((MCTSNodeV2 *)(this->children_[action_index]))->DoMonteCarloTreeSearchOnce(state, selection_strategy, simulation_strategy);
    }
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}
