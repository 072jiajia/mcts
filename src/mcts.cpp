
#include "mcts.h"

Agent::Agent(Algo algo, double time_limit_ms, int min_iter, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, int num_threads)
	: algo_(algo), timer_(), time_limit_ms_(time_limit_ms),
	  min_iter_(min_iter),
	  selection_strategy_(selection_strategy),
	  simulation_strategy_(simulation_strategy),
	  num_threads_(num_threads)
{
	if (!selection_strategy_)
		selection_strategy_ = new UCBHighest();

	if (!simulation_strategy_)
		simulation_strategy_ = new SimulationDefaultStrategy();
}

Agent::~Agent() {}

void *Agent::LaunchSearchThread(void *args_void)
{
	RootParallelInput *args = (RootParallelInput *)args_void;
	MCTSNode *root = args->root();
	Game *b = args->b();
	Timer *timer = args->timer();
	int min_iter = args->min_iter();
	double time_limit_ms = args->time_limit_ms();
	SelectionStrategy *selection_strategy = args->selection_strategy();
	SimulationStrategy *simulation_strategy = args->simulation_strategy();

	while (root->GetTotalSimulationCount() < min_iter ||
		   timer->get_duration() < time_limit_ms / 1000.)
	{
		Game *b_clone = b->Clone();
		root->SearchOnce(b_clone, selection_strategy, simulation_strategy);
		delete b_clone;
	}
	delete args;
	pthread_exit(NULL);
}

Action *Agent::SearchAction(Game *b)
{
	timer_.reset();
	if (algo_ == Algo::MCTS_COPY_STATE)
	{
		MCTSNodeCS *mcts_root = new MCTSNodeCS(b);
		while (mcts_root->GetTotalSimulationCount() < min_iter_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			mcts_root->SearchOnce(selection_strategy_, simulation_strategy_);
		}
		int best_move = mcts_root->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root->GetTotalSimulationCount() << std::endl;
		delete mcts_root;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (algo_ == Algo::MCTS)
	{
		MCTSNode *mcts_root = new MCTSNode();
		while (mcts_root->GetTotalSimulationCount() < min_iter_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			Game *b_clone = b->Clone();
			mcts_root->SearchOnce(b_clone, selection_strategy_, simulation_strategy_);
			delete b_clone;
		}
		int best_move = mcts_root->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root->GetTotalSimulationCount() << std::endl;
		delete mcts_root;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (algo_ == Algo::RAVE)
	{
		RaveNode *mcts_root = new RaveNode();
		while (mcts_root->GetTotalSimulationCount() < min_iter_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			std::vector<int> self_action;
			std::vector<int> oppo_action;
			Game *b_clone = b->Clone();
			mcts_root->SearchOnce(b_clone, selection_strategy_, simulation_strategy_, self_action, oppo_action);
			delete b_clone;
		}
		int best_move = mcts_root->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root->GetTotalSimulationCount() << std::endl;
		delete mcts_root;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (algo_ == Algo::MCTS_LEAF_PARALLEL)
	{
		MCTSNode *mcts_root = new MCTSNode();
		while (mcts_root->GetTotalSimulationCount() < min_iter_ ||
			   timer_.get_duration() < time_limit_ms_ / 1000.)
		{
			Game *b_clone = b->Clone();
			mcts_root->SearchOnce(b_clone, selection_strategy_, simulation_strategy_);
			delete b_clone;
		}
		int best_move = mcts_root->ChooseMoveWithMostFrequency();
		std::cout << "Total Search Times: " << mcts_root->GetTotalSimulationCount() << std::endl;
		delete mcts_root;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
	else if (algo_ == Algo::MCTS_ROOT_PARALLEL)
	{
		MCTSNode **mcts_root_nodes = new MCTSNode *[num_threads_];
		pthread_t *t = new pthread_t[num_threads_];

		for (int i = 0; i < num_threads_; i++)
		{
			mcts_root_nodes[i] = new MCTSNode();
			RootParallelInput *data = new RootParallelInput(b, mcts_root_nodes[i], &timer_, min_iter_, time_limit_ms_,
															selection_strategy_, simulation_strategy_);
			if (pthread_create(&t[i], NULL, LaunchSearchThread, (void *)data) != 0)
			{
				perror("pthread_create() error");
				exit(1);
			}
		}

		for (int i = 0; i < num_threads_; i++)
		{
			pthread_join(t[i], NULL);
			std::cout << "Total Search Times: " << mcts_root_nodes[i]->GetTotalSimulationCount() << std::endl;
		}

		std::vector<int> selected_counts = mcts_root_nodes[0]->GetFrequencies();

		for (int i = 1; i < num_threads_; i++)
		{
			std::vector<int> temp = mcts_root_nodes[i]->GetFrequencies();
			for (int j = 0; j < temp.size(); j++)
			{
				selected_counts[j] += temp[j];
			}
		}

		int best_move = -1;
		int best_value = -1;
		for (int i = 0; i < selected_counts.size(); i++)
		{
			if (best_value < selected_counts[i])
			{
				best_move = i;
				best_value = selected_counts[i];
			}
		}

		for (int i = 0; i < num_threads_; i++)
		{
			delete mcts_root_nodes[i];
		}
		delete mcts_root_nodes;
		delete t;

		ActionList *action_list = b->GetLegalMoves();
		Action *output = action_list->Pop(best_move);
		delete action_list;
		return output;
	}
}
