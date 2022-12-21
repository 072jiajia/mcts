#include "node.h"

MCTSNodeImpl_::MCTSNodeImpl_() : Q_(rand() / RAND_MAX * 0.001), N_(0), expanded_(false) {}

MCTSNodeImpl_::~MCTSNodeImpl_()
{
    for (uint i = 0; i < children_.size(); i++)
    {
        delete children_[i];
    }
}

float MCTSNodeImpl_::GetValueForCurrentPlayer() { return Q_; }

float MCTSNodeImpl_::GetTotalSimulationCount() { return N_; }

float MCTSNodeImpl_::Q() { return Q_; }
float MCTSNodeImpl_::N() { return N_; }
bool MCTSNodeImpl_::IsExpanded() { return expanded_; }

const std::vector<MCTSNode_ *> *MCTSNodeImpl_::GetChildren() const { return &children_; }

int MCTSNodeImpl_::ChooseMoveWithMostFrequency()
{
    float best_value = -1e20;
    int best_move = -1;
    for (uint i = 0; i < children_.size(); i++)
    {
        float value = children_[i]->N();

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}

float MCTSNodeImpl_::EvaluateGameOverNode(Game *state)
{
    if (this->N_ == 0)
        this->Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
    this->N_++;
    return -this->Q_;
}

Game *MCTSNodeCS::GetCurrentState() { return state_; }

MCTSNodeCS::MCTSNodeCS(Game *s) : MCTSNodeImpl_(), state_(s->Clone()) {}

MCTSNodeCS::~MCTSNodeCS() { delete state_; }

void MCTSNodeCS::Expansion()
{
    ActionList *movable_actions = state_->GetLegalMoves();

    for (uint i = 0; i < movable_actions->GetSize(); i++)
    {
        MCTSNodeCS *new_node = new MCTSNodeCS(state_);
        new_node->state_->DoAction(movable_actions->Get(i));
        this->children_.push_back(new_node);
    }

    this->expanded_ = true;

    delete movable_actions;
}

float MCTSNodeCS::SearchOnce(SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state_->IsGameOver())
        return EvaluateGameOverNode(state_);
    if (this->N_ == 0)
    {
        this->Q_ = simulation_strategy->SimulationOnce(state_);
        this->N_ = 1;
        return -this->Q_;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->expanded_)
        this->Expansion();

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    MCTSNodeCS *child = (MCTSNodeCS *)(this->children_[action_index]);
    float q = child->SearchOnce(selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}

MCTSNode::MCTSNode() : MCTSNodeImpl_(), actions_(nullptr) {}

MCTSNode::~MCTSNode()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
}

void MCTSNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->children_.push_back(new MCTSNode());
    }
    this->expanded_ = true;
}

float MCTSNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
        return EvaluateGameOverNode(state);
    if (this->N_ == 0)
    {
        this->Q_ = simulation_strategy->SimulationOnce(state);
        this->N_ = 1;
        return -this->Q_;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */
    if (!this->expanded_)
        this->Expansion(state);

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    state->DoAction(actions_->Get(action_index));
    MCTSNode *child = (MCTSNode *)(this->children_[action_index]);
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    this->Q_ += (q - this->Q_) / ++this->N_;
    return -q;
}

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
        RootParallelInput *data = new RootParallelInput(state_, roots_[i], time_controller,
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
        std::cout << "Total Search Times: " << roots_[i]->GetTotalSimulationCount() << std::endl;
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
    RootParallelInput *args = (RootParallelInput *)args_void;
    MCTSNode *root = args->root();
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

MCTSMutexNode::MCTSMutexNode() : MCTSNodeImpl_(), virtual_N_(0), actions_(nullptr), lock()
{
    sem_init(&lock, 0, 1);
}

MCTSMutexNode::~MCTSMutexNode()
{
    if (actions_ != nullptr)
    {
        delete actions_;
    }
}

sem_t *MCTSMutexNode::GetLock()
{
    return &lock;
}

void MCTSMutexNode::Expansion(Game *state)
{
    actions_ = state->GetLegalMoves();

    for (uint i = 0; i < actions_->GetSize(); i++)
    {
        this->children_.push_back(new MCTSMutexNode());
    }
    this->expanded_ = true;
}

float MCTSMutexNode::SearchOnce(Game *state, SelectionStrategy *selection_strategy, SimulationStrategy *simulation_strategy)
{
    /* Simulation for leaf nodes */
    if (state->IsGameOver())
    {
        sem_wait(&lock);

        if (this->N_ == 0)
            this->Q_ = EvaluateResult(state, state->GetPlayerThisTurn());
        this->N_++;
        this->virtual_N_--;
        sem_post(&lock);
        float output = -this->Q_;
        return output;
    }
    if (this->N_ == 0)
    {
        sem_wait(&lock);
        this->Q_ = simulation_strategy->SimulationOnce(state);
        this->N_ = 1;
        this->virtual_N_--;

        sem_post(&lock);
        float output = -this->Q_;
        return output;
    }

    /* Instead of expanding the children 1-by-1
       expand all of them at once */

    sem_wait(&lock);
    if (!this->expanded_)
        this->Expansion(state);
    sem_post(&lock);

    /* Recursively select the child nodes */
    int action_index = selection_strategy->Select(this);
    state->DoAction(actions_->Get(action_index));
    MCTSMutexNode *child = (MCTSMutexNode *)(this->children_[action_index]);
    float q = child->SearchOnce(state, selection_strategy, simulation_strategy);

    /* Back Propagation */
    sem_wait(&lock);
    this->Q_ += (q - this->Q_) / ++this->N_;
    this->virtual_N_--;
    sem_post(&lock);

    return -q;
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
        RootParallelInputMutex *data = new RootParallelInputMutex(state_, root_, time_controller,
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

void MCTSMutexNode::NPlusPlus()
{
    N_++;
}

void *MCTSParallelTree::LaunchSearchThread(void *args_void)
{
    RootParallelInputMutex *args = (RootParallelInputMutex *)args_void;
    MCTSMutexNode *root = args->root();
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
