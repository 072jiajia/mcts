#pragma once
#include <random>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>

#include "../utils.h"

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

struct ParallelDataContainer
{
    ParallelDataContainer(SimulationStrategy *strategy, Game *b, float *result_ptr)
    {
        strategy_ = strategy;
        b_ = b;
        result_ptr_ = result_ptr;
    }
    SimulationStrategy *strategy() { return strategy_; }
    Game *b() { return b_; }
    float *result_ptr() { return result_ptr_; }

    SimulationStrategy *strategy_;
    Game *b_;
    float *result_ptr_;
};

class ParallelSimulationStrategy : public SimulationStrategy
{
public:
    ParallelSimulationStrategy(int num_threads, SimulationStrategy *strategy);
    ~ParallelSimulationStrategy();
    static void *LaunchThread(void *args);
    float SimulationOnce(Game *b) const;

private:
    SimulationStrategy *strategy_;
    int num_threads_;

    float *results;
    pthread_t *t;
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
