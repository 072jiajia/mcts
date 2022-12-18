#include "node.h"

float MCTSNode_::GetValueForCurrentPlayer() { return Q_; }

float MCTSNode_::GetTotalSimulationCount() { return N_; }

float MCTSNode_::Q() { return Q_; }
float MCTSNode_::N() { return N_; }
bool MCTSNode_::IsExpanded() { return expanded_; }

std::vector<MCTSNode_ *> MCTSNode_::GetChildren() const { return children_; }

MCTSNode_::MCTSNode_() : Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

MCTSNode_::~MCTSNode_()
{
    for (uint i = 0; i < children_.size(); i++)
    {
        delete children_[i];
    }
}

int MCTSNode_::ChooseMoveWithMostFrequency()
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

std::vector<int> MCTSNode_::GetFrequencies()
{
    std::vector<int> output;
    for (uint i = 0; i < children_.size(); i++)
    {
        output.push_back(children_[i]->N_);
    }
    return output;
}

float MCTSNode_::EvaluateGameOverNode(Game *state)
{
    if (this->N_ == 0)
        this->Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
    this->N_++;
    return -this->Q_;
}

Game *MCTSNodeCS::GetCurrentState() { return state_; }

MCTSNodeCS::MCTSNodeCS(Game *s) : MCTSNode_(), state_(s->Clone()) {}

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

float MCTSNodeCS::DoMonteCarloTreeSearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
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
    float q = child->DoMonteCarloTreeSearchOnce(selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}

MCTSNode::MCTSNode() : MCTSNode_(), actions_(nullptr) {}

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

float MCTSNode::DoMonteCarloTreeSearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
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
    float q = child->DoMonteCarloTreeSearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}
