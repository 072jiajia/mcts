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

enum class Algorithm
{
	MCTS,
	MCTS_COPY_STATE,
	RAVE,
	MCTS_LEAF_PARALLEL,
	MCTS_ROOT_PARALLEL,
};

struct RootParallelInput
{
	RootParallelInput(Game *b, MCTSNode *root, Timer *timer, int min_iter, double time_limit_ms,
					  SelectionStrategy *selection_strategy,
					  SimulationStrategy *simulation_strategy)
	{
		b_ = b;
		root_ = root;
		timer_ = timer;
		min_iter_ = min_iter;
		time_limit_ms_ = time_limit_ms;
		selection_strategy_ = selection_strategy;
		simulation_strategy_ = simulation_strategy;
	}

	Game *b() { return b_; }
	MCTSNode *root() { return root_; }
	Timer *timer() { return timer_; }
	int min_iter() { return min_iter_; }
	double time_limit_ms() { return time_limit_ms_; }
	SelectionStrategy *selection_strategy() { return selection_strategy_; }
	SimulationStrategy *simulation_strategy() { return simulation_strategy_; }

private:
	Game *b_;
	MCTSNode *root_;
	Timer *timer_;
	int min_iter_;
	double time_limit_ms_;
	SelectionStrategy *selection_strategy_;
	SimulationStrategy *simulation_strategy_;
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

	static void *LaunchSearchThread(void *args_void);

private:
	Algorithm algo_;
	double time_limit_ms_;
	int min_iter_;
	Timer timer_;
	SelectionStrategy *selection_strategy_;
	SimulationStrategy *simulation_strategy_;
	int num_threads_;
};