
#include "mcts.h"

Agent::Agent(AgentOptions &options)
	: algo_(options.algo()), timer_(), time_limit_ms_(options.time_limit_ms()),
	  min_iter_(options.min_iter()),
	  decision_strategy_(options.decision_strategy()),
	  num_threads_(options.num_threads()),
	  num_processes_(options.num_processes())
{
	SelectionStrategy *selection_strategy(options.selection_strategy());
	SimulationStrategy *simulation_strategy(options.simulation_strategy());
	PolicyStrategy *policy_strategy(options.policy_strategy());

	if (!selection_strategy)
		selection_strategy = new UCBHighest();

	if (!simulation_strategy)
		simulation_strategy = new SimulationDefaultStrategy();

	if (!decision_strategy_)
		decision_strategy_ = new MostFrequency();

	if (!policy_strategy)
		policy_strategy = new DefaultPolicy();

	if (algo_ == Algorithm::MCTS_COPY_STATE)
	{
		search_strategy_ = new MCTSNodeCSSearcher(selection_strategy, simulation_strategy);
	}
	else if (algo_ == Algorithm::MCTS)
	{
		search_strategy_ = new MCTSNodeSearcher(selection_strategy, simulation_strategy);
	}
	else if (algo_ == Algorithm::RAVE)
	{
		search_strategy_ = new RaveNodeSearcher(selection_strategy, simulation_strategy);
	}
	else if (algo_ == Algorithm::MCTS_PUCT)
	{
		search_strategy_ = new PolicyNodeSearcher(selection_strategy, simulation_strategy, policy_strategy);
		policy_strategy = nullptr;
	}
	else if (algo_ == Algorithm::MCTS_VIRTUAL_LOSS)
	{
		search_strategy_ = new MutexNodeSearcher(selection_strategy, simulation_strategy);
	}
	else
	{
		throw std::invalid_argument("Unknown Algorithm");
	}
	if (policy_strategy)
		delete policy_strategy;
}

Agent::~Agent() {}

Action *Agent::SearchAction(Game *b)
{
	timer_.reset();
	TimeControlStrategy *time_controller = new CountDown(&timer_, time_limit_ms_);
	MCTSTree_ *mcts_tree;
	if (num_threads_ == 1)
	{
		MCTSNode_ *mcts_root = search_strategy_->CreateNode(b);
		mcts_tree = new SingleTree(mcts_root, b);
	}
	else
	{
		if (algo_ == Algorithm::MCTS_VIRTUAL_LOSS)
		{
			MCTSNode_ *mcts_root = search_strategy_->CreateNode(b);
			mcts_tree = new MultiThreadSingleTree(mcts_root, b, num_threads_);
		}
		else
		{
			MCTSNode_ **mcts_roots = new MCTSNode_ *[num_threads_];
			for (int i = 0; i < num_threads_; i++)
			{
				mcts_roots[i] = search_strategy_->CreateNode(b);
			}
			mcts_tree = new ThreadParallel(mcts_roots, b, num_threads_);
		}
	}

	if (num_processes_ > 1)
	{
		mcts_tree = new ProcessParallel(b, mcts_tree, num_processes_);
	}

	mcts_tree->Search(search_strategy_, time_controller);
	int best_move = mcts_tree->MakeDecision(decision_strategy_);
	std::cout << "Total Search Times: " << mcts_tree->GetTotalSimulationCount() << std::endl;
	std::cout << "Action's Value: " << -mcts_tree->GetValues()[best_move] << std::endl;
	delete mcts_tree;

	ActionList *action_list = b->GetLegalMoves();
	Action *output = action_list->Pop(best_move);
	delete action_list;
	return output;
}
