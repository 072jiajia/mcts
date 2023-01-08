#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#include "node.h"
#include "tree.h"
#include "utils.h"
#include "strategies/simulation.h"
#include "strategies/selection.h"
#include "strategies/policy.h"
#include "strategies/timecontrol.h"
#include "strategies/decision.h"
#include "strategies/node_searcher.h"

void *LaunchPonder(void *args_void);

struct PonderInput
{
    PonderInput(sem_t *sem, MCTSTree_ *tree,
                TimeControlStrategy *time_controller,
                NodeSearcher_ *search_strategy)
        : sem_(sem), tree_(tree),
          time_controller_{time_controller},
          search_strategy_{search_strategy} {}

    THREAD_INPUT_ARG(sem_t *, sem);
    THREAD_INPUT_ARG(MCTSTree_ *, tree);
    THREAD_INPUT_ARG(TimeControlStrategy *, time_controller);
    THREAD_INPUT_ARG(NodeSearcher_ *, search_strategy);
};

class PonderHandler
{
public:
    PonderHandler();
    void StopPondering();
    void StartPondering(MCTSTree_ *tree, NodeSearcher_ *search_strategy_);

private:
    pthread_t ponder_thread_;
    bool ponder_stop_;
    sem_t ponder_sem_;
};

struct AgentOptions
{
    AgentOptions() {}
    OPTION_ARG(double, time_limit_ms) = 1000.;
    OPTION_ARG(int, min_iter) = 1000;
    OPTION_POINTER_ARG(NodeSearcher_ *, search_strategy) = nullptr;
    OPTION_POINTER_ARG(DecisionStrategy *, decision_strategy) = nullptr;
    OPTION_ARG(int, num_threads) = 1;
    OPTION_ARG(int, num_processes) = 1;
    OPTION_ARG(bool, moving_root) = false;
    OPTION_ARG(bool, does_ponder) = false;
};

class Agent
{
public:
    Agent(AgentOptions &options);
    ~Agent();

    Action *SearchAction(const Game *state);
    MCTSTree_ *CreateTree(const Game *state) const;
    void HandleOppenentMove(const Action *);

private:
    double time_limit_ms_;
    int min_iter_;
    NodeSearcher_ *search_strategy_;
    DecisionStrategy *decision_strategy_;
    MCTSTree_ *mcts_tree_;
    int num_threads_;
    int num_processes_;
    bool moving_root_;

    bool does_ponder_;
    PonderHandler *ponder_handler_;
};
