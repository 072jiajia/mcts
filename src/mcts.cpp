
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

Action *MCTSAgent::SearchAction(Game *b, std::string method)
{
	timer_.reset();
	if (method == "MCTSNode")
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

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (method == "MCTSNodeV2")
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

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (method == "Rave")
	{
		RaveNode *mcts_root_node = new RaveNode();
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			std::vector<int> self_action;
			std::vector<int> oppo_action;
			mcts_root_node->DoMonteCarloTreeSearchOnce(b->Clone(), selection_strategy_, simulation_strategy_, self_action, oppo_action);
		}
		int best_move = mcts_root_node->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root_node->GetTotalSimulationCount() << std::endl;
		delete mcts_root_node;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
}
