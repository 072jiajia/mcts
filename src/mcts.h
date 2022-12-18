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

class Agent
{
public:
	enum class Algo
	{
		MCTS,
		MCTS_COPY_STATE,
		RAVE,
		MCTS_LEAF_PARALLEL,
		MCTS_ROOT_PARALLEL,
	};

	Agent(Algo algo,
		  double time_limit_ms,
		  int min_iterations,
		  SelectionStrategy *selection_strategy = nullptr,
		  SimulationStrategy *simulation_strategy = nullptr,
		  int num_threads = 1);
	virtual ~Agent();

	Action *SearchAction(Game *b);

	static void *LaunchSearchThread(void *args_void);

private:
	Algo algo_;
	double time_limit_ms_;
	int min_iterations_;
	Timer timer_;
	SelectionStrategy *selection_strategy_;
	SimulationStrategy *simulation_strategy_;
	int num_threads_;
};