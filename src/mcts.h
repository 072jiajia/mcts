#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "node.h"
#include "rave.h"
#include "utils.h"
#include "simulation.h"
#include "selection.h"

class Agent
{
public:
	enum class Algo
	{
		MCTS,
		MCTS_COPY_STATE,
		RAVE
	};

	Agent(Algo algo,
		  double time_limit_ms,
		  int min_iterations,
		  SelectionStrategy *selection_strategy = nullptr,
		  SimulationStrategy *simulation_strategy = nullptr);
	~Agent();

	Action *SearchAction(Game *b);

private:
	Algo algo_;
	double time_limit_ms_;
	int min_iterations_;
	Timer timer_;
	SelectionStrategy *selection_strategy_;
	SimulationStrategy *simulation_strategy_;
};