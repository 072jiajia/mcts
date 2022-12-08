#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "node.h"
#include "utils.h"

template <typename G>
class MCTSPlayer
{
private:
	double time_limit_ms_;
	int min_iterations_;
	Timer timer_;

public:
	MCTSPlayer(double time_limit_ms = 1000., int min_iterations = 100000) : timer_(), time_limit_ms_(time_limit_ms), min_iterations_(min_iterations) {}
	~MCTSPlayer() {}

	typename G::Action SearchAction(G *b)
	{
		timer_.reset();
		mcts_node<G> *mcts_root_node = new mcts_node<G>(b);
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			mcts_root_node->DoMonteCarloTreeSearchOnce();
		}
		int best_move = mcts_root_node->ChooseMoveWithMostFrequency();
		std::cout << mcts_root_node->GetTotalSimulationCount() << std::endl;
		delete mcts_root_node;
		return b->GetMovableActions()[best_move];
	}
};