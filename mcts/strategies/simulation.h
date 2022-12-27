#pragma once
#include <random>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>
#include <vector>

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

class QuickRandomRollout : public SimulationStrategy
{
public:
    QuickRandomRollout(ActionList *action_list);
    ~QuickRandomRollout();

    float SimulationOnce(Game *b) const;

private:
    ActionList *action_list_;
    std::vector<int> *indices_;

    inline Action *GetRandomMove(Game *b) const;
};
