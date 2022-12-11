#pragma once
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include <memory>
#include <algorithm>

#include "simulation.h"

template <typename G>
class MCTSNodeV2
{
    /* The difference between MCTSNode & MCTSNodeV2 is that
     * instead of saving the state in the node,
     * MCTSNodeV2 takes a mutable state as an input and
     * does actions on that state while doing monte carlo tree search.
     */
public:
    float GetValueForCurrentPlayer() { return Q_; }
    float GetTotalSimulationCount() { return N_; }
    bool IsExpanded() { return expanded_; }

    MCTSNodeV2() : Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

    ~MCTSNodeV2()
    {
        for (uint i = 0; i < children_.size(); i++)
        {
            delete children_[i];
        }
    }

    void Expansion(G *state)
    {
        actions_ = state->GetLegalMoves();

        for (uint i = 0; i < actions_.size(); i++)
        {
            children_.push_back(new MCTSNodeV2<G>());
        }

        expanded_ = true;
    }

    float DoMonteCarloTreeSearchOnce(G *state, SimulationStrategy<G> *simulation_strategy)
    {
        float q;
        if (state->IsGameOver())
        {
            if (N_ == 0)
                Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
            N_++;
            return -Q_;
        }
        else if (N_ == 0)
        {
            q = simulation_strategy->SimulationOnce(state);
        }
        else
        {
            if (!expanded_)
                this->Expansion(state);

            int action_index = this->SelecHighestUCBChildNode();
            state->DoAction(actions_[action_index]);
            q = children_[action_index]->DoMonteCarloTreeSearchOnce(state, simulation_strategy);
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

    std::vector<typename G::Action> actions_;
    std::vector<MCTSNodeV2<G> *> children_;
    float Q_;
    float N_;
    bool expanded_;
};
