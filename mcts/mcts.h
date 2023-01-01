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
#include "strategies/timecontrol.h"
#include "strategies/decision.h"
#include "strategies/search.h"

enum class Algorithm
{
	MCTS,
	MCTS_COPY_STATE,
	RAVE,
	MCTS_PUCT,
	MCTS_VIRTUAL_LOSS
};

struct AgentOptions
{
	AgentOptions(Algorithm algo)
	{
		algo_ = algo;
	}
	OPTION_ARG(Algorithm, algo);
	OPTION_ARG(double, time_limit_ms) = 1000.;
	OPTION_ARG(int, min_iter) = 1000;
	OPTION_POINTER_ARG(SelectionStrategy *, selection_strategy) = nullptr;
	OPTION_POINTER_ARG(SimulationStrategy *, simulation_strategy) = nullptr;
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
	MCTSNode_ *CreateNode(const Algorithm &algo, Game *state);

	Algorithm algo_;
	double time_limit_ms_;
	int min_iter_;
	Timer timer_;
	SearchStrategy *search_strategy_;
	// SelectionStrategy *selection_strategy_;
	// SimulationStrategy *simulation_strategy_;
	DecisionStrategy *decision_strategy_;
	int num_threads_;
	int num_processes_;
};