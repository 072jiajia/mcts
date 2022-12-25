#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>

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

int MCTSTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
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

int MCTSTreeCS::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
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

int MCTSParallelTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
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

int RaveTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
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

RootParallel::RootParallel(Game *state, MCTSTree_ *tree, int num_processes) : tree_(tree), num_processes_(num_processes)
{
    ActionList *action_list = state->GetLegalMoves();
    action_size_ = action_list->GetSize();

    shm_id_ = shmget(0, sizeof(int) * num_processes * action_size_, IPC_CREAT | 0600);
    if (shm_id_ == -1)
    {
        perror("shmget: shmget failed");
        exit(1);
    }

    shm_ = static_cast<int *>(shmat(shm_id_, NULL, 0));
    if (shm_ == (int *)-1)
    {
        perror("shmat: attach error");
        exit(1);
    }
}

RootParallel::~RootParallel()
{
    if (shmdt(shm_) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(1);
    }
    shmctl(shm_id_, IPC_RMID, NULL);
    delete tree_;
}

float RootParallel::GetTotalSimulationCount()
{
    int total_counts = 0;
    for (int i = 0; i < num_processes_ * action_size_; i++)
    {
        total_counts += shm_[i];
    }
    return total_counts;
}

void RootParallel::Search(SelectionStrategy *selection_strategy,
                          SimulationStrategy *simulation_strategy,
                          TimeControlStrategy *time_controller)
{
    for (int process_id = 0; process_id < num_processes_; process_id++)
    {
        pid_t pid;
        pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Fork Failed");
            exit(-1);
        }
        else if (pid == 0)
        {
            srand(getpid());
            tree_->Search(selection_strategy, simulation_strategy, time_controller);

            std::vector<int> freqs = tree_->GetFrequencies();
            for (int i = 0; i < freqs.size(); i++)
            {
                shm_[process_id * action_size_ + i] = freqs[i];
            }

            exit(EXIT_SUCCESS);
        }
    }

    for (int i = 0; i < num_processes_; i++)
    {
        wait(NULL);
    }

    for (int j = 0; j < num_processes_; j++)
    {
        for (int i = 0; i < action_size_; i++)
        {
            std::cout << shm_[j * action_size_ + i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int RootParallel::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> RootParallel::GetFrequencies()
{
    std::vector<int> output(action_size_, 0);
    for (int i = 0; i < action_size_; i++)
    {
        int sum = 0;
        for (int j = 0; j < num_processes_; j++)
        {
            sum += shm_[j * action_size_ + i];
        }
        output[i] = sum;
    }
    return output;
}
