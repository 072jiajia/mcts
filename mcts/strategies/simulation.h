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
    virtual float SimulationOnce(Game *state) const = 0;
};

class SimulationDefaultStrategy : public SimulationStrategy
{
public:
    float SimulationOnce(Game *state) const;

private:
    Action *GetRandomMove(Game *state) const;
};

struct LeadParallelInput
{
    LeadParallelInput(SimulationStrategy *strategy, Game *state, float *result_ptr)
        : strategy_(strategy), state_(state), result_ptr_(result_ptr) {}

    THREAD_INPUT_ARG(SimulationStrategy *, strategy);
    THREAD_INPUT_ARG(Game *, state);
    THREAD_INPUT_ARG(float *, result_ptr);
};

class ParallelSimulationStrategy : public SimulationStrategy
{
public:
    ParallelSimulationStrategy(int num_threads, SimulationStrategy *strategy);
    ~ParallelSimulationStrategy();
    static void *LaunchThread(void *args);
    float SimulationOnce(Game *state) const;

private:
    SimulationStrategy *strategy_;
    int num_threads_;

    float *results;
    pthread_t *t;
};

class QuickRandomRollout : public SimulationStrategy
{
public:
    QuickRandomRollout(ActionList *action_space);
    ~QuickRandomRollout();

    float SimulationOnce(Game *state) const;

private:
    ActionList *action_space_;
    std::vector<int> *indices_;

    inline Action *GetRandomMove(Game *state) const;
};
