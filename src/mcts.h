#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "node.h"
#include "utils.h"

template <typename G>
class MCTSAgent
{
public:
	MCTSAgent(double time_limit_ms = 1000., int min_iterations = 100000, SimulationStrategy<G> *simulation_strategy = nullptr)
		: timer_(), time_limit_ms_(time_limit_ms), min_iterations_(min_iterations),
		  simulation_strategy_(simulation_strategy)
	{
		if (!simulation_strategy_)
			simulation_strategy_ = new SimulationDefaultStrategy<G>();
	}
	~MCTSAgent() {}

	typename G::Action SearchAction(G *b)
	{
		timer_.reset();
		MCTSNode<G> *mcts_root_node = new MCTSNode<G>(b);
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			mcts_root_node->DoMonteCarloTreeSearchOnce(simulation_strategy_);
		}
		int best_move = mcts_root_node->ChooseMoveWithMostFrequency();
		std::cout << mcts_root_node->GetTotalSimulationCount() << std::endl;
		delete mcts_root_node;
		return b->GetLegalMoves()[best_move];
	}

private:
	double time_limit_ms_;
	int min_iterations_;
	Timer timer_;
	SimulationStrategy<G> *simulation_strategy_;
};