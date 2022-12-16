
#include "mcts.h"

Agent::Agent(Algo algo, double time_limit_ms, int min_iterations, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
	: algo_(algo), timer_(), time_limit_ms_(time_limit_ms),
	  min_iterations_(min_iterations),
	  selection_strategy_(selection_strategy),
	  simulation_strategy_(simulation_strategy)
{
	if (!selection_strategy_)
		selection_strategy_ = new UCBHighest();

	if (!simulation_strategy_)
		simulation_strategy_ = new SimulationDefaultStrategy();
}

Agent::~Agent() {}

Action *Agent::SearchAction(Game *b)
{
	timer_.reset();
	if (algo_ == Algo::MCTS_COPY_STATE)
	{
		MCTSNodeCS *mcts_root_node = new MCTSNodeCS(b);
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
	else if (algo_ == Algo::MCTS)
	{
		MCTSNode *mcts_root_node = new MCTSNode();
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			Game *b_clone = b->Clone();
			mcts_root_node->DoMonteCarloTreeSearchOnce(b_clone, selection_strategy_, simulation_strategy_);
			delete b_clone;
		}
		int best_move = mcts_root_node->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root_node->GetTotalSimulationCount() << std::endl;
		delete mcts_root_node;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (algo_ == Algo::RAVE)
	{
		RaveNode *mcts_root_node = new RaveNode();
		while (mcts_root_node->GetTotalSimulationCount() < min_iterations_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			std::vector<int> self_action;
			std::vector<int> oppo_action;
			Game *b_clone = b->Clone();
			mcts_root_node->DoMonteCarloTreeSearchOnce(b_clone, selection_strategy_, simulation_strategy_, self_action, oppo_action);
			delete b_clone;
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
