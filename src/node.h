#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "simulation.h"

template <typename G>
class MCTSNode
{
public:
    float GetValueForCurrentPlayer() { return Q_; }
    float GetTotalSimulationCount() { return N_; }
    G *GetCurrentState() { return state_; }
    bool IsExpanded() { return expanded_; }

    MCTSNode(G *s)
        : state_(s->Clone()), Q_(rand() / RAND_MAX * 0.001), N_(0),
          expanded_(false) {}

    ~MCTSNode()
    {
        for (uint i = 0; i < children_.size(); i++)
        {
            delete children_[i];
        }
        delete state_;
    }

    void Expansion()
    {
        auto movable_actions = state_->GetLegalMoves();

        for (uint i = 0; i < movable_actions.size(); i++)
        {
            MCTSNode *new_node = new MCTSNode<G>(state_);
            new_node->state_->DoAction(movable_actions[i]);
            children_.push_back(new_node);
        }

        expanded_ = true;
    }

    float DoMonteCarloTreeSearchOnce(SimulationStrategy<G> *simulation_strategy)
    {
        float q;
        if (state_->IsGameOver())
        {
            if (N_ == 0)
                Q_ = EvaluateResult(state_, state_->GetPlayerThisTurn());
            N_++;
            return -Q_;
        }
        else if (N_ == 0)
        {
            q = simulation_strategy->SimulationOnce(state_);
        }
        else
        {
            if (!expanded_)
                this->Expansion();
            q = children_[this->SelecHighestUCBChildNode()]->DoMonteCarloTreeSearchOnce(simulation_strategy);
        }
        Q_ += (q - Q_) / ++N_;
        return -q;
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

private:
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

    G *state_;
    std::vector<MCTSNode<G> *> children_;
    float Q_;
    float N_;
    bool expanded_;
};
