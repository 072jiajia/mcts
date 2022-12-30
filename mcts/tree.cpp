#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <iomanip>

#include "tree.h"

SingleTree::SingleTree(MCTSNode_ *root, Game *state) : root_(root), state_(state->Clone()) {}

SingleTree::~SingleTree()
{
    delete state_;
    delete root_;
}

float SingleTree::GetTotalSimulationCount()
{
    return root_->N();
}

void SingleTree::Search(SelectionStrategy *selection_strategy,
                        SimulationStrategy *simulation_strategy,
                        TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        SearchParam temp{state_, selection_strategy, simulation_strategy};
        root_->SearchOnce(&temp);
    }
}

int SingleTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> SingleTree::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<float> SingleTree::GetValues()
{
    return root_->GetChildrenQ();
}

ThreadParallel::ThreadParallel(MCTSNode_ **roots, Game *state, int num_threads) : roots_(roots), state_(state), num_threads_(num_threads)
{
    threads_ = new pthread_t[num_threads];
}

ThreadParallel::~ThreadParallel()
{
    for (int i = 0; i < num_threads_; i++)
    {
        delete roots_[i];
    }
    delete[] roots_;
    delete threads_;
}

float ThreadParallel::GetTotalSimulationCount()
{
    int total_counts = 0;
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSNode_ *root = roots_[i];
        total_counts += root->N();
    }
    return total_counts;
}

void ThreadParallel::Search(SelectionStrategy *selection_strategy,
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

int ThreadParallel::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

void *LaunchSearchThread(void *args_void)
{
    MCTSThreadInput *args = (MCTSThreadInput *)args_void;
    MCTSNode_ *root = args->root();
    Game *b = args->b();
    TimeControlStrategy *time_controller = args->time_controller();
    SelectionStrategy *selection_strategy = args->selection_strategy();
    SimulationStrategy *simulation_strategy = args->simulation_strategy();

    while (!time_controller->Stop())
    {
        SearchParam temp{b, selection_strategy, simulation_strategy};
        root->SearchOnce(&temp);
    }
    delete args;
    pthread_exit(NULL);
}

std::vector<int> ThreadParallel::GetFrequencies()
{
    std::vector<int> output = roots_[0]->GetChildrenN();

    for (int i = 1; i < num_threads_; i++)
    {
        std::vector<int> temp = roots_[i]->GetChildrenN();
        for (int j = 0; j < temp.size(); j++)
        {
            output[j] += temp[j];
        }
    }

    return output;
}

std::vector<float> ThreadParallel::GetValues()
{
    std::vector<float> output = roots_[0]->GetChildrenQ();

    for (int i = 1; i < num_threads_; i++)
    {
        std::vector<float> temp = roots_[i]->GetChildrenQ();
        for (int j = 0; j < temp.size(); j++)
        {
            output[j] += temp[j];
        }
    }

    return output;
}

MultiThreadSingleTree::MultiThreadSingleTree(MCTSNode_ *root, Game *state, int num_threads) : root_(root), state_(state), num_threads_(num_threads)
{
    threads_ = new pthread_t[num_threads];
}

MultiThreadSingleTree::~MultiThreadSingleTree()
{
    delete root_;
    delete threads_;
}

float MultiThreadSingleTree::GetTotalSimulationCount()
{
    return root_->N();
}

void MultiThreadSingleTree::Search(SelectionStrategy *selection_strategy,
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

int MultiThreadSingleTree::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> MultiThreadSingleTree::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<float> MultiThreadSingleTree::GetValues()
{
    return root_->GetChildrenQ();
}

ProcessParallel::ProcessParallel(Game *state, MCTSTree_ *tree, int num_processes) : tree_(tree), num_processes_(num_processes)
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

    shm_value_id_ = shmget(0, sizeof(float) * num_processes * action_size_, IPC_CREAT | 0600);
    if (shm_value_id_ == -1)
    {
        perror("shmget: shmget failed");
        exit(1);
    }

    shm_value_ = static_cast<float *>(shmat(shm_value_id_, NULL, 0));
    if (shm_value_ == (float *)-1)
    {
        perror("shmat: attach error");
        exit(1);
    }
}

ProcessParallel::~ProcessParallel()
{
    if (shmdt(shm_) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(1);
    }
    shmctl(shm_id_, IPC_RMID, NULL);

    if (shmdt(shm_value_) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(1);
    }
    shmctl(shm_value_id_, IPC_RMID, NULL);
    delete tree_;
}

float ProcessParallel::GetTotalSimulationCount()
{
    int total_counts = 0;
    for (int i = 0; i < num_processes_ * action_size_; i++)
    {
        total_counts += shm_[i];
    }
    return total_counts;
}

void ProcessParallel::Search(SelectionStrategy *selection_strategy,
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
            std::vector<float> values = tree_->GetValues();
            for (int i = 0; i < freqs.size(); i++)
            {
                shm_[process_id * action_size_ + i] = freqs[i];
                shm_value_[process_id * action_size_ + i] = values[i];
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

int ProcessParallel::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> ProcessParallel::GetFrequencies()
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

std::vector<float> ProcessParallel::GetValues()
{
    std::vector<float> output(action_size_, 0);
    for (int i = 0; i < action_size_; i++)
    {
        float sum = 0;
        for (int j = 0; j < num_processes_; j++)
        {
            sum += shm_value_[j * action_size_ + i];
        }
        output[i] = sum;
    }
    return output;
}
