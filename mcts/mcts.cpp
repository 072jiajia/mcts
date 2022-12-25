
#include "mcts.h"

Agent::Agent(AgentOptions &options)
	: algo_(options.algo()), timer_(), time_limit_ms_(options.time_limit_ms()),
	  min_iter_(options.min_iter()),
	  selection_strategy_(options.selection_strategy()),
	  simulation_strategy_(options.simulation_strategy()),
	  decision_strategy_(options.decision_strategy()),
	  num_threads_(options.num_threads()),
	  num_processes_(options.num_processes())
{
	if (!selection_strategy_)
		selection_strategy_ = new UCBHighest();

	if (!simulation_strategy_)
		simulation_strategy_ = new SimulationDefaultStrategy();

	if (!decision_strategy_)
		decision_strategy_ = new MostFrequency();
}

Agent::~Agent() {}

Action *Agent::SearchAction(Game *b)
{
	timer_.reset();
	TimeControlStrategy *time_controller = new CountDown(&timer_, time_limit_ms_);
	MCTSTree_ *mcts_root;
	if (algo_ == Algorithm::MCTS_COPY_STATE)
	{
		mcts_root = new MCTSTreeCS(b);
	}
	else if (algo_ == Algorithm::MCTS)
	{
		mcts_root = new MCTSTree(b);
	}
	else if (algo_ == Algorithm::RAVE)
	{
		mcts_root = new RaveTree(b);
	}
	else if (algo_ == Algorithm::MCTS_LEAF_PARALLEL)
	{
		mcts_root = new MCTSTree(b);
	}
	else if (algo_ == Algorithm::MCTS_TREE_PARALLEL)
	{
		mcts_root = new MCTSParallelTree(b, num_threads_);
	}
	else
	{
		throw std::invalid_argument("Unknown Algorithm");
	}

	if (num_processes_ > 1)
	{
		mcts_root = new RootParallel(b, mcts_root, num_processes_);
	}

	mcts_root->Search(selection_strategy_, simulation_strategy_, time_controller);
	int best_move = mcts_root->MakeDecision(decision_strategy_);
	std::cout << "Total Search Times: " << mcts_root->GetTotalSimulationCount() << std::endl;
	delete mcts_root;

	ActionList *action_list = b->GetLegalMoves();
	Action *output = action_list->Pop(best_move);
	delete action_list;
	return output;
}