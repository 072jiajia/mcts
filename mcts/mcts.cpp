
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

MCTSNode_ *Agent::CreateNode(const Algorithm &algo, Game *state)
{
	if (algo == Algorithm::MCTS_COPY_STATE)
	{
		return new MCTSNodeCS(state);
	}
	else if (algo == Algorithm::MCTS)
	{
		return new MCTSNode();
	}
	else if (algo == Algorithm::RAVE)
	{
		return new RaveNode();
	}
	else if (algo == Algorithm::MCTS_PUCT)
	{
		return new MCTSPolicyNode();
	}
	throw std::invalid_argument("Unknown Algorithm");
}

Action *Agent::SearchAction(Game *b)
{
	timer_.reset();
	TimeControlStrategy *time_controller = new CountDown(&timer_, time_limit_ms_);
	MCTSTree_ *mcts_tree;
	if (num_threads_ == 1)
	{
		MCTSNode_ *mcts_root = CreateNode(algo_, b);
		mcts_tree = new SingleTree(mcts_root, b);
	}
	else
	{
		if (algo_ == Algorithm::MCTS_VIRTUAL_LOSS)
		{
			MCTSNode_ *mcts_root = CreateNode(algo_, b);
			mcts_tree = new MultiThreadSingleTree(mcts_root, b, num_threads_);
		}
		else
		{
			MCTSNode_ **mcts_roots = new MCTSNode_ *[num_threads_];
			for (int i = 0; i < num_threads_; i++)
			{
				mcts_roots[i] = CreateNode(algo_, b);
			}
			mcts_tree = new ThreadParallel(mcts_roots, b, num_threads_);
		}
	}

	if (num_processes_ > 1)
	{
		mcts_tree = new ProcessParallel(b, mcts_tree, num_processes_);
	}

	mcts_tree->Search(selection_strategy_, simulation_strategy_, time_controller);
	int best_move = mcts_tree->MakeDecision(decision_strategy_);
	std::cout << "Total Search Times: " << mcts_tree->GetTotalSimulationCount() << std::endl;
	delete mcts_tree;

	ActionList *action_list = b->GetLegalMoves();
	Action *output = action_list->Pop(best_move);
	delete action_list;
	return output;
}
