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
};

class Agent
{
public:
    Agent(AgentOptions &options);
    ~Agent();

    Action *SearchAction(Game *b);

private:
    double time_limit_ms_;
    int min_iter_;
    Timer timer_;
    NodeSearcher_ *search_strategy_;
    DecisionStrategy *decision_strategy_;
    int num_threads_;
    int num_processes_;
};