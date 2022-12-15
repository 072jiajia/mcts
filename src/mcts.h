#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "node.h"
#include "rave.h"
#include "utils.h"
#include "simulation.h"
#include "selection.h"

class MCTSAgent
{
public:
	MCTSAgent(double time_limit_ms,
			  int min_iterations,
			  SelectionStrategy *selection_strategy = nullptr,
			  SimulationStrategy *simulation_strategy = nullptr);
	~MCTSAgent();

	Action *SearchAction(Game *b, std::string method = "MCTSNode");

private:
	double time_limit_ms_;
	int min_iterations_;
	Timer timer_;
	SelectionStrategy *selection_strategy_;
	SimulationStrategy *simulation_strategy_;
};