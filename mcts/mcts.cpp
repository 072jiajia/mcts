
#include "mcts.h"

void *LaunchPonder(void *args_void)
{
	PonderInput *args = (PonderInput *)args_void;
	sem_t *sem = args->sem();
	MCTSTree_ *tree = args->tree();
	TimeControlStrategy *time_controller = args->time_controller();
	NodeSearcher_ *search_strategy = args->search_strategy();

	if (dynamic_cast<ProcessParallel *>(tree))
		throw std::runtime_error("ProcessParallel doesn't support pondering");

	tree->Search(search_strategy, time_controller);

	sem_post(sem);
	delete time_controller;
	delete args;
	pthread_exit(NULL);
}

PonderHandler::PonderHandler()
	: ponder_thread_(), ponder_stop_(true), ponder_sem_() {}

void PonderHandler::StopPondering()
{
	if (ponder_stop_)
		return;

	ponder_stop_ = true;
	sem_wait(&ponder_sem_);
}

void PonderHandler::StartPondering(MCTSTree_ *tree, NodeSearcher_ *search_strategy_)
{
	ponder_stop_ = false;
	TimeControlStrategy *time_controller = new SignalStopper(&ponder_stop_);
	sem_init(&ponder_sem_, 0, 0);
	PonderInput *data = new PonderInput(&ponder_sem_, tree, time_controller, search_strategy_);
	if (pthread_create(&ponder_thread_, NULL, LaunchPonder, (void *)data) != 0)
	{
		perror("pthread_create() error");
		exit(1);
	}
}

Agent::Agent(AgentOptions &options)
	: time_limit_ms_(options.time_limit_ms()),
	  min_iter_(options.min_iter()),
	  search_strategy_(options.search_strategy()),
	  decision_strategy_(options.decision_strategy()),
	  mcts_tree_(nullptr),
	  num_threads_(options.num_threads()),
	  num_processes_(options.num_processes()),
	  moving_root_(options.moving_root()),
	  does_ponder_(options.does_ponder())
{
	if (!decision_strategy_)
		decision_strategy_ = new MostFrequency();

	if (!search_strategy_)
		throw std::invalid_argument("search_strategy not defined");

	if (does_ponder_)
		ponder_handler_ = new PonderHandler();

	if (does_ponder_ && num_processes_ > 1)
		throw std::runtime_error("ProcessParallel doesn't support pondering");
}

Agent::~Agent()
{
	delete search_strategy_;
	delete decision_strategy_;

	if (!mcts_tree_)
	{
		delete mcts_tree_;
	}
}

MCTSTree_ *Agent::CreateTree(const Game *state) const
{
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
	return mcts_tree;
}

Action *Agent::SearchAction(const Game *state)
{
	TimeControlStrategy *time_controller = new CountDown(time_limit_ms_);

	if (does_ponder_)
	{
		ponder_handler_->StopPondering();
	}

	if (!mcts_tree_)
	{
		mcts_tree_ = CreateTree(state);
	}
	else
	{
		if (!mcts_tree_->GetState()->IsSame(state))
		{
			std::cout << "Moving Root Not Matched (Rebuild Tree)" << std::endl;
			delete mcts_tree_;
			mcts_tree_ = CreateTree(state);
		}
		else
		{
			std::cout << "Precomputed " << mcts_tree_->GetTotalSimulationCount() << " times" << std::endl;
		}
	}

	mcts_tree_->Search(search_strategy_, time_controller);
	int best_move = mcts_tree_->MakeDecision(decision_strategy_);
	std::cout << "Total Search Times: " << mcts_tree_->GetTotalSimulationCount() << std::endl;
	std::cout << "Action's Value: " << -mcts_tree_->GetValues()[best_move] << std::endl;

	if (moving_root_)
	{
		mcts_tree_->MoveRoot(best_move);
		if (does_ponder_)
		{
			ponder_handler_->StartPondering(mcts_tree_, search_strategy_);
		}
	}
	else
	{
		delete mcts_tree_;
		mcts_tree_ = nullptr;
	}

	ActionList *action_list = state->GetLegalMoves();
	Action *output = action_list->Pop(best_move);
	delete action_list;
	return output;
}

void Agent::HandleOppenentMove(const Action *action)
{
	if (!mcts_tree_)
		return;
	Game *state = mcts_tree_->GetState();
	ActionList *action_list = state->GetLegalMoves();
	int action_index = -1;
	for (int i = 0; i < action_list->GetSize(); i++)
	{
		if (action_list->Get(i)->IsSame(action))
		{
			action_index = i;
		}
	}
	delete action_list;

	if (does_ponder_)
	{
		ponder_handler_->StopPondering();
	}

	if (action_index == -1)
	{
		std::cout << "Agent::HandleOppenentMove action not found" << std::endl;
		delete mcts_tree_;
		mcts_tree_ = nullptr;
	}
	else
	{
		mcts_tree_->MoveRoot(action_index);
		if (does_ponder_)
		{
			ponder_handler_->StartPondering(mcts_tree_, search_strategy_);
		}
	}
}
