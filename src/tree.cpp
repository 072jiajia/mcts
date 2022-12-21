#include "tree.h"

MCTSTree::MCTSTree(Game *state) : state_(state->Clone())
{
    root_ = new MCTSNode();
}

MCTSTree::~MCTSTree()
{
    delete state_;
    delete root_;
}

float MCTSTree::GetTotalSimulationCount()
{
    return root_->N();
}

void MCTSTree::Search(SelectionStrategy *selection_strategy,
                      SimulationStrategy *simulation_strategy,
                      TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        Game *cloned_state = state_->Clone();
        root_->SearchOnce(cloned_state, selection_strategy, simulation_strategy);
        delete cloned_state;
    }
}

int MCTSTree::MakeDecision()
{
    std::vector<int> freq = GetFrequencies();
    int best_move = -1;
    int best_value = -1;
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] > best_value)
        {
            best_value = freq[i];
            best_move = i;
        }
    }
    return best_move;
}

MCTSTreeCS::MCTSTreeCS(Game *state) : state_(state->Clone())
{
    root_ = new MCTSNodeCS(state_);
}

MCTSTreeCS::~MCTSTreeCS()
{
    delete state_;
    delete root_;
}

float MCTSTreeCS::GetTotalSimulationCount()
{
    return root_->N();
}

void MCTSTreeCS::Search(SelectionStrategy *selection_strategy,
                        SimulationStrategy *simulation_strategy,
                        TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        root_->SearchOnce(selection_strategy, simulation_strategy);
    }
}

int MCTSTreeCS::MakeDecision()
{
    std::vector<int> freq = GetFrequencies();
    int best_move = -1;
    int best_value = -1;
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] > best_value)
        {
            best_value = freq[i];
            best_move = i;
        }
    }
    return best_move;
}

MCTSMultiTree::MCTSMultiTree(Game *state, int num_threads) : state_(state), num_threads_(num_threads)
{
    roots_ = new MCTSNode *[num_threads];
    threads_ = new pthread_t[num_threads];

    for (int i = 0; i < num_threads; i++)
    {
        roots_[i] = new MCTSNode();
    }
}

MCTSMultiTree::~MCTSMultiTree()
{
    for (int i = 0; i < num_threads_; i++)
    {
        delete roots_[i];
    }
    delete roots_;
    delete threads_;
}

float MCTSMultiTree::GetTotalSimulationCount()
{
    int total_counts = 0;
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSNode *root = roots_[i];
        total_counts += root->N();
    }
    return total_counts;
}

void MCTSMultiTree::Search(SelectionStrategy *selection_strategy,
                           SimulationStrategy *simulation_strategy,
                           TimeControlStrategy *time_controller)
{
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSThreadInput *data = new MCTSThreadInput(state_, roots_[i], time_controller,
                                                    selection_strategy, simulation_strategy);
        if (pthread_create(&threads_[i], NULL, LaunchSearchThread, (void *)data) != 0)
        {
            perror("pthread_create() error");
            exit(1);
        }
    }

    for (int i = 0; i < num_threads_; i++)
    {
        pthread_join(threads_[i], NULL);
    }
}

int MCTSMultiTree::MakeDecision()
{
    std::vector<int> freq = GetFrequencies();
    int best_move = -1;
    int best_value = -1;
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] > best_value)
        {
            best_value = freq[i];
            best_move = i;
        }
    }
    return best_move;
}

void *MCTSMultiTree::LaunchSearchThread(void *args_void)
{
    MCTSThreadInput *args = (MCTSThreadInput *)args_void;
    MCTSNode *root = (MCTSNode *)(args->root());
    Game *b = args->b();
    TimeControlStrategy *time_controller = args->time_controller();
    SelectionStrategy *selection_strategy = args->selection_strategy();
    SimulationStrategy *simulation_strategy = args->simulation_strategy();

    while (!time_controller->Stop())
    {
        Game *b_clone = b->Clone();
        root->SearchOnce(b_clone, selection_strategy, simulation_strategy);
        delete b_clone;
    }
    delete args;
    pthread_exit(NULL);
}

std::vector<int> MCTSTree::GetFrequencies()
{
    std::vector<int> output;
    const std::vector<MCTSNode_ *> *children = root_->GetChildren();
    for (int i = 0; i < children->size(); i++)
    {
        MCTSNode_ *node = children->at(i);
        output.push_back(node->N());
    }
    return output;
}

std::vector<int> MCTSTreeCS::GetFrequencies()
{
    std::vector<int> output;
    const std::vector<MCTSNode_ *> *children = root_->GetChildren();
    for (int i = 0; i < children->size(); i++)
    {
        MCTSNode_ *node = children->at(i);
        output.push_back(node->N());
    }
    return output;
}

std::vector<int> MCTSMultiTree::GetFrequencies()
{
    std::vector<int> output;
    const std::vector<MCTSNode_ *> *children = roots_[0]->GetChildren();
    for (int i = 0; i < children->size(); i++)
    {
        MCTSNode_ *node = children->at(i);
        output.push_back(node->N());
    }

    for (int i = 1; i < num_threads_; i++)
    {
        const std::vector<MCTSNode_ *> *children = roots_[i]->GetChildren();
        for (int j = 0; j < children->size(); j++)
        {
            MCTSNode_ *node = children->at(j);
            output[j] += node->N();
        }
    }

    return output;
}

MCTSParallelTree::MCTSParallelTree(Game *state, int num_threads) : state_(state), num_threads_(num_threads)
{
    root_ = new MCTSMutexNode();
    threads_ = new pthread_t[num_threads];
}

MCTSParallelTree::~MCTSParallelTree()
{
    delete root_;
    delete threads_;
}

float MCTSParallelTree::GetTotalSimulationCount()
{
    return root_->N();
}

void MCTSParallelTree::Search(SelectionStrategy *selection_strategy,
                              SimulationStrategy *simulation_strategy,
                              TimeControlStrategy *time_controller)
{
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSThreadInput *data = new MCTSThreadInput(state_, root_, time_controller,
                                                    selection_strategy, simulation_strategy);
        if (pthread_create(&threads_[i], NULL, LaunchSearchThread, (void *)data) != 0)
        {
            perror("pthread_create() error");
            exit(1);
        }
    }

    for (int i = 0; i < num_threads_; i++)
    {
        pthread_join(threads_[i], NULL);
    }
}

int MCTSParallelTree::MakeDecision()
{
    std::vector<int> freq = GetFrequencies();
    int best_move = -1;
    int best_value = -1;
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] > best_value)
        {
            best_value = freq[i];
            best_move = i;
        }
    }
    return best_move;
}

void *MCTSParallelTree::LaunchSearchThread(void *args_void)
{
    MCTSThreadInput *args = (MCTSThreadInput *)args_void;
    MCTSMutexNode *root = (MCTSMutexNode *)(args->root());
    Game *b = args->b();
    TimeControlStrategy *time_controller = args->time_controller();
    SelectionStrategy *selection_strategy = args->selection_strategy();
    SimulationStrategy *simulation_strategy = args->simulation_strategy();

    while (!time_controller->Stop())
    {
        Game *b_clone = b->Clone();
        root->NPlusPlus();
        root->SearchOnce(b_clone, selection_strategy, simulation_strategy);
        delete b_clone;
    }
    delete args;
    pthread_exit(NULL);
}

std::vector<int> MCTSParallelTree::GetFrequencies()
{
    std::vector<int> output;
    const std::vector<MCTSNode_ *> *children = root_->GetChildren();
    for (int i = 0; i < children->size(); i++)
    {
        MCTSNode_ *node = children->at(i);
        output.push_back(node->N());
    }
    return output;
}

RaveTree::RaveTree(Game *state) : state_(state->Clone())
{
    root_ = new RaveNode();
}

RaveTree::~RaveTree()
{
    delete state_;
    delete root_;
}

float RaveTree::GetTotalSimulationCount()
{
    return root_->N();
}

void RaveTree::Search(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy, TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        std::vector<int> self_action;
        std::vector<int> oppo_action;
        Game *cloned_state = state_->Clone();
        root_->SearchOnce(cloned_state, selection_strategy, simulation_strategy, self_action, oppo_action);
        delete cloned_state;
    }
}

int RaveTree::MakeDecision()
{
    std::vector<int> freq = GetFrequencies();
    int best_move = -1;
    int best_value = -1;
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] > best_value)
        {
            best_value = freq[i];
            best_move = i;
        }
    }
    return best_move;
}

std::vector<int> RaveTree::GetFrequencies()
{
    std::vector<int> output;
    const std::vector<MCTSNode_ *> *children = root_->GetChildren();
    for (int i = 0; i < children->size(); i++)
    {
        MCTSNode_ *node = children->at(i);
        output.push_back(node->N());
    }
    return output;
}
