#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>

#include "node.h"
#include "rave.h"
#include "utils.h"
#include "simulation.h"
#include "selection.h"
#include "timecontrol.h"

enum class Algorithm
{
	MCTS,
	MCTS_COPY_STATE,
	RAVE,
	MCTS_LEAF_PARALLEL,
	MCTS_ROOT_PARALLEL,
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
	OPTION_ARG(int, num_threads) = 1;
};

class Agent
{
public:
	Agent(AgentOptions &options);
	virtual ~Agent();

	Action *SearchAction(Game *b);

private:
	Algorithm algo_;
	double time_limit_ms_;
	int min_iter_;
	Timer timer_;
	SelectionStrategy *selection_strategy_;
	SimulationStrategy *simulation_strategy_;
	int num_threads_;
};