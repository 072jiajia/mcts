
#include "mcts.h"

Agent::Agent(AgentOptions &options)
	: timer_(), time_limit_ms_(options.time_limit_ms()),
	  min_iter_(options.min_iter()),
	  search_strategy_(options.search_strategy()),
	  decision_strategy_(options.decision_strategy()),
	  num_threads_(options.num_threads()),
	  num_processes_(options.num_processes())
{
	if (!decision_strategy_)
		decision_strategy_ = new MostFrequency();

	if (!search_strategy_)
		throw std::invalid_argument("search_strategy not defined");
}

Agent::~Agent()
{
	delete search_strategy_;
	delete decision_strategy_;
}

Action *Agent::SearchAction(Game *state)
{
	timer_.reset();
	TimeControlStrategy *time_controller = new CountDown(&timer_, time_limit_ms_);
	MCTSTree_ *mcts_tree;
	if (num_threads_ == 1)
	{
		MCTSNode_ *mcts_root = search_strategy_->CreateNode(state);
		mcts_tree = new SingleTree(mcts_root, state);
	}
	else
	{
		if (search_strategy_->DoesSupportTreeParallel())
		{
			MCTSNode_ *mcts_root = search_strategy_->CreateNode(state);
			mcts_tree = new TreeParallel(mcts_root, state, num_threads_);
		}
		else
		{
			MCTSNode_ **mcts_roots = new MCTSNode_ *[num_threads_];
			for (int i = 0; i < num_threads_; i++)
			{
				mcts_roots[i] = search_strategy_->CreateNode(state);
			}
			mcts_tree = new RootParallel(mcts_roots, state, num_threads_);
		}
	}

	if (num_processes_ > 1)
	{
		mcts_tree = new ProcessParallel(state, mcts_tree, num_processes_);
	}

	mcts_tree->Search(search_strategy_, time_controller);
	int best_move = mcts_tree->MakeDecision(decision_strategy_);
	std::cout << "Total Search Times: " << mcts_tree->GetTotalSimulationCount() << std::endl;
	std::cout << "Action's Value: " << -mcts_tree->GetValues()[best_move] << std::endl;
	delete mcts_tree;

	ActionList *action_list = state->GetLegalMoves();
	Action *output = action_list->Pop(best_move);
	delete action_list;
	return output;
}
