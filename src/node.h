#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "simulation.h"

template <typename G>
class MCTSNodeBase
{
public:
    float GetValueForCurrentPlayer() { return Q_; }
    float GetTotalSimulationCount() { return N_; }
    bool IsExpanded() { return expanded_; }

    MCTSNodeBase() : Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

    ~MCTSNodeBase()
    {
        for (uint i = 0; i < children_.size(); i++)
        {
            delete children_[i];
        }
    }

    int ChooseMoveWithMostFrequency()
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

protected:
    int SelecHighestUCBChildNode(float C = 1.4)
    {
        float best_value = -1e20;
        int best_move = -1;
        float logN = std::log(N_ + 1.);
        for (uint i = 0; i < children_.size(); i++)
        {
            float value = -children_[i]->Q_ + C * std::sqrt(logN / (children_[i]->N_ + 1));

            if (value > best_value)
            {
                best_value = value;
                best_move = i;
            }
        }
        return best_move;
    }

    std::vector<MCTSNodeBase<G> *> children_;
    float Q_;
    float N_;
    bool expanded_;
};

template <typename G>
class MCTSNode : public MCTSNodeBase<G>
{
public:
    G *GetCurrentState() { return state_; }

    MCTSNode(G *s) : MCTSNodeBase<G>(), state_(s->Clone()) {}

    ~MCTSNode() { delete state_; }

    void Expansion()
    {
        auto movable_actions = state_->GetLegalMoves();

        for (uint i = 0; i < movable_actions.size(); i++)
        {
            MCTSNode *new_node = new MCTSNode<G>(state_);
            new_node->state_->DoAction(movable_actions[i]);
            this->children_.push_back(new_node);
        }

        this->expanded_ = true;
    }

    float DoMonteCarloTreeSearchOnce(SimulationStrategy<G> *simulation_strategy)
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
            q = ((MCTSNode<G> *)(this->children_[this->SelecHighestUCBChildNode()]))->DoMonteCarloTreeSearchOnce(simulation_strategy);
        }
        this->Q_ += (q - this->Q_) / ++this->N_;
        return -q;
    }

protected:
    G *state_;
};

template <typename G>
class MCTSNodeV2 : public MCTSNodeBase<G>
{
    /* The difference between MCTSNode & MCTSNodeV2 is that
     * instead of saving the state in the node,
     * MCTSNodeV2 takes a mutable state as an input and
     * does actions on that state while doing monte carlo tree search.
     */
public:
    MCTSNodeV2() : MCTSNodeBase<G>() {}

    ~MCTSNodeV2() {}

    void Expansion(G *state)
    {
        actions_ = state->GetLegalMoves();

        for (uint i = 0; i < actions_.size(); i++)
        {
            this->children_.push_back(new MCTSNodeV2<G>());
        }
        this->expanded_ = true;
    }

    float DoMonteCarloTreeSearchOnce(G *state, SimulationStrategy<G> *simulation_strategy)
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

            int action_index = this->SelecHighestUCBChildNode();
            state->DoAction(actions_[action_index]);
            q = ((MCTSNodeV2<G> *)(this->children_[action_index]))->DoMonteCarloTreeSearchOnce(state, simulation_strategy);
        }
        this->Q_ += (q - this->Q_) / ++this->N_;
        return -q;
    }

protected:
    std::vector<typename G::Action> actions_;
};
