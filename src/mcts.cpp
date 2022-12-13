
#include "mcts.h"

MCTSAgent::MCTSAgent(double time_limit_ms, int min_iterations, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
	: timer_(), time_limit_ms_(time_limit_ms),
	  min_iterations_(min_iterations),
	  selection_strategy_(selection_strategy),
	  simulation_strategy_(simulation_strategy)
{
	if (!selection_strategy_)
		selection_strategy_ = new UCBHighest();

	if (!simulation_strategy_)
		simulation_strategy_ = new SimulationDefaultStrategy();
}

MCTSAgent::~MCTSAgent() {}

Action *MCTSAgent::SearchAction(Game *b, bool use_v2)
{
	timer_.reset();
	if (!use_v2)
	{
		MCTSNode *mcts_root_node = new MCTSNode(b);
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			mcts_root_node->DoMonteCarloTreeSearchOnce(selection_strategy_, simulation_strategy_);
		}
		int best_move = mcts_root_node->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root_node->GetTotalSimulationCount() << std::endl;
		delete mcts_root_node;
		return b->GetLegalMoves()[best_move];
	}
	else
	{
		MCTSNodeV2 *mcts_root_node = new MCTSNodeV2();
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			mcts_root_node->DoMonteCarloTreeSearchOnce(b->Clone(), selection_strategy_, simulation_strategy_);
		}
		int best_move = mcts_root_node->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root_node->GetTotalSimulationCount() << std::endl;
		delete mcts_root_node;
		return b->GetLegalMoves()[best_move];
	}
}
