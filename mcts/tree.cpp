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

SingleTree::SingleTree(MCTSNode_ *root, const Game *state) : root_(root), state_(state->Clone()) {}

SingleTree::~SingleTree()
{
    delete state_;
    delete root_;
}

float SingleTree::GetTotalSimulationCount()
{
    return root_->N();
}

void SingleTree::Search(NodeSearcher_ *search_strategy,
                        TimeControlStrategy *time_controller)
{
    while (!time_controller->Stop())
    {
        root_->SearchOnce(state_, search_strategy);
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

void SingleTree::MoveRoot(int index)
{
    MCTSNode_ *original_root = root_;
    root_ = root_->PopChild(index);
    delete original_root;

    ActionList *action_list = state_->GetLegalMoves();
    state_->DoAction(action_list->Get(index));
    delete action_list;
}

Game *SingleTree::GetState() { return state_; }

RootParallel::RootParallel(MCTSNode_ **roots, const Game *state, int num_threads) : roots_(roots), state_(state->Clone()), num_threads_(num_threads)
{
    threads_ = new pthread_t[num_threads];
}

RootParallel::~RootParallel()
{
    for (int i = 0; i < num_threads_; i++)
    {
        delete roots_[i];
    }
    delete state_;
    delete[] roots_;
    delete threads_;
}

float RootParallel::GetTotalSimulationCount()
{
    int total_counts = 0;
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSNode_ *root = roots_[i];
        total_counts += root->N();
    }
    return total_counts;
}

void RootParallel::Search(NodeSearcher_ *search_strategy,
                          TimeControlStrategy *time_controller)
{
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSThreadInput *data = new MCTSThreadInput(state_, roots_[i], time_controller,
                                                    search_strategy);
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

int RootParallel::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

void *LaunchSearchThread(void *args_void)
{
    MCTSThreadInput *args = (MCTSThreadInput *)args_void;
    MCTSNode_ *root = args->root();
    Game *state = args->state();
    TimeControlStrategy *time_controller = args->time_controller();
    NodeSearcher_ *search_strategy = args->search_strategy();

    while (!time_controller->Stop())
    {
        root->SearchOnce(state, search_strategy);
    }
    delete args;
    pthread_exit(NULL);
}

std::vector<int> RootParallel::GetFrequencies()
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

std::vector<float> RootParallel::GetValues()
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

void RootParallel::MoveRoot(int index)
{
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSNode_ *original_root = roots_[i];
        roots_[i] = roots_[i]->PopChild(index);
        delete original_root;
    }

    ActionList *action_list = state_->GetLegalMoves();
    state_->DoAction(action_list->Get(index));
    delete action_list;
}

Game *RootParallel::GetState() { return state_; }

TreeParallel::TreeParallel(MCTSNode_ *root, const Game *state, int num_threads) : root_(root), state_(state->Clone()), num_threads_(num_threads)
{
    threads_ = new pthread_t[num_threads];
}

TreeParallel::~TreeParallel()
{
    delete state_;
    delete root_;
    delete threads_;
}

float TreeParallel::GetTotalSimulationCount()
{
    return root_->N();
}

void TreeParallel::Search(NodeSearcher_ *search_strategy,
                          TimeControlStrategy *time_controller)
{
    for (int i = 0; i < num_threads_; i++)
    {
        MCTSThreadInput *data = new MCTSThreadInput(state_, root_, time_controller,
                                                    search_strategy);
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

int TreeParallel::MakeDecision(DecisionStrategy *decision_strategy)
{
    return decision_strategy->Choose(this);
}

std::vector<int> TreeParallel::GetFrequencies()
{
    return root_->GetChildrenN();
}

std::vector<float> TreeParallel::GetValues()
{
    return root_->GetChildrenQ();
}

void TreeParallel::MoveRoot(int index)
{
    MCTSNode_ *original_root = root_;
    root_ = root_->PopChild(index);
    delete original_root;

    ActionList *action_list = state_->GetLegalMoves();
    state_->DoAction(action_list->Get(index));
    delete action_list;
}

Game *TreeParallel::GetState() { return state_; }

void ProcessParallel::PrepareSHM()
{
    if (shm_usable_)
        throw std::runtime_error("Shared memory reallocated");

    ActionList *action_list = state_->GetLegalMoves();
    action_size_ = action_list->GetSize();
    delete action_list;

    shm_id_ = shmget(0, sizeof(int) * num_processes_ * action_size_, IPC_CREAT | 0600);
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

    shm_value_id_ = shmget(0, sizeof(float) * num_processes_ * action_size_, IPC_CREAT | 0600);
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

    shm_usable_ = true;
}

void ProcessParallel::ClearSHM()
{
    if (!shm_usable_)
        return;

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

    shm_usable_ = false;
}

ProcessParallel::ProcessParallel(const Game *state, MCTSTree_ *tree, int num_processes)
    : tree_(tree), state_(state->Clone()), num_processes_(num_processes),
      shm_usable_(false)
{
}

ProcessParallel::~ProcessParallel()
{
    delete tree_;
    delete state_;
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

void ProcessParallel::Search(NodeSearcher_ *search_strategy,
                             TimeControlStrategy *time_controller)
{
    ClearSHM();
    PrepareSHM();
    for (int process_id = 1; process_id < num_processes_; process_id++)
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
            tree_->Search(search_strategy, time_controller);

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

    /* main-process */
    {
        tree_->Search(search_strategy, time_controller);
        std::vector<int> freqs = tree_->GetFrequencies();
        std::vector<float> values = tree_->GetValues();
        for (int i = 0; i < freqs.size(); i++)
        {
            shm_[i] = freqs[i];
            shm_value_[i] = values[i];
        }
    }

    for (int i = 1; i < num_processes_; i++)
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
        output[i] = sum / num_processes_;
    }
    return output;
}

void ProcessParallel::MoveRoot(int index)
{
    tree_->MoveRoot(index);

    ActionList *action_list = state_->GetLegalMoves();
    state_->DoAction(action_list->Get(index));
    delete action_list;
}

Game *ProcessParallel::GetState() { return tree_->GetState(); }
