#pragma once
#include <random>
#include "utils.h"

class SimulationStrategy
{
public:
    virtual ~SimulationStrategy() = default;
    virtual float SimulationOnce(Game *b) const = 0;
};

class SimulationDefaultStrategy : public SimulationStrategy
{
public:
    float SimulationOnce(Game *b) const;

private:
    Action *GetRandomMove(Game *b) const;
};

// class QuickRandomRollout : public SimulationStrategy
// {
// public:
//     QuickRandomRollout(std::vector<Action *> *action_space)
//         : action_space_(action_space){};

//     ~QuickRandomRollout() { delete action_space_; }

// public:
//     float SimulationOnce(Game *b) const override
//     {
//         Game *traverse_b = b->Clone();
//         Player turn = traverse_b->GetPlayerThisTurn();
//         while (!traverse_b->IsGameOver())
//         {
//             Action *move = GetRandomMove(b);
//             traverse_b->DoAction(move);
//         }
//         return EvaluateResult(traverse_b, turn);
//     }

// private:
//     std::vector<Action *> *action_space_;

//     inline Action *GetRandomMove(Game *b) const
//     {
//         std::random_shuffle(action_space_->begin(), action_space_->end());

//         for (int i = 0; i < action_space_->size(); i++)
//         {
//             Action *action = action_space_->at(i);
//             if (Game::IsMovable(*b, action))
//                 return action;
//         }

//         throw std::invalid_argument("QuickRandomRollout::GetRandomMove, No valid actions are found");
//     }
// };
