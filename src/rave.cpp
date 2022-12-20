#include "rave.h"

RaveNode::RaveNode() : MCTSNode(), rave_QN_() {}

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
        this->children_.push_back(new RaveNode());
    }
    this->expanded_ = true;
}

float RaveNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, std::vector<int> &self_action, std::vector<int> &oppo_action)
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
    Action *selected_action = actions_->Get(action_index);
    state->DoAction(selected_action);
    RaveNode *child = (RaveNode *)(this->children_[action_index]);
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy, oppo_action, self_action);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;

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

RaveRoot::RaveRoot(Game *state) : state_(state->Clone())
{
    root_ = new RaveNode();
}

RaveRoot::~RaveRoot()
{
    delete state_;
    delete root_;
}

float RaveRoot::GetTotalSimulationCount()
{
    return root_->N();
}

void RaveRoot::Search(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        std::vector<int> self_action;
        std::vector<int> oppo_action;
        Game *cloned_state = state_->Clone();
        root_->SearchOnce(cloned_state, selection_strategy, simulation_strategy, self_action, oppo_action);
        delete cloned_state;
    }
}

int RaveRoot::MakeDecision()
{
    return root_->ChooseMoveWithMostFrequency();
}
