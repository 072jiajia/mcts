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
    void Ponder();
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
    pthread_t ponder_thread_;
    bool ponder_stop_;
    sem_t ponder_sem_;
};