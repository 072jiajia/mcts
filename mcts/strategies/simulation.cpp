#include "simulation.h"

float SimulationDefaultStrategy::SimulationOnce(Game *b) const
{
    Game *traverse_b = b->Clone();
    Player turn = traverse_b->GetPlayerThisTurn();
    while (!traverse_b->IsGameOver())
    {
        Action *move = this->GetRandomMove(traverse_b);
        traverse_b->DoAction(move);
        delete move;
    }
    float output = EvaluateResult(traverse_b, turn);
    delete traverse_b;
    return output;
}

Action *SimulationDefaultStrategy::GetRandomMove(Game *b) const
{
    ActionList *movable_actions = b->GetLegalMoves();
    int m = rand() % movable_actions->GetSize();
    Action *output = movable_actions->Pop(m);
    delete movable_actions;
    return output;
}

ParallelSimulationStrategy::ParallelSimulationStrategy(int num_threads, SimulationStrategy *strategy)
    : num_threads_(num_threads), strategy_(strategy)
{
    std::cout << "[Warning] Attempting to Use ParallelSimulationStrategy" << std::endl;
    std::cout << "This method is not recommended since creating threads may take more time than simulation" << std::endl;
    results = new float[num_threads];
    t = new pthread_t[num_threads];
}

ParallelSimulationStrategy::~ParallelSimulationStrategy()
{
    delete strategy_;
    delete results;
    delete t;
}

void *ParallelSimulationStrategy::LaunchThread(void *void_args)
{
    ParallelDataContainer *args = (ParallelDataContainer *)void_args;
    SimulationStrategy *strategy = args->strategy();
    Game *b = args->b();
    float *result_ptr = args->result_ptr();

    *result_ptr = strategy->SimulationOnce(b);

    delete args;
    pthread_exit(NULL);
}

float ParallelSimulationStrategy::SimulationOnce(Game *b) const
{

    for (int i = 0; i < num_threads_; i++)
    {
        ParallelDataContainer *data = new ParallelDataContainer(strategy_, b, &(results[i]));
        pthread_create(&t[i], NULL, LaunchThread, (void *)data);
    }

    float output = 0;
    for (int i = 0; i < num_threads_; i++)
    {
        pthread_join(t[i], NULL);
        output += results[i];
    }

    return output / num_threads_;
}

QuickRandomRollout::QuickRandomRollout(ActionList *action_list)
    : action_list_(action_list), indices_(new std::vector<int>())
{
    for (int i = 0; i < action_list->GetSize(); i++)
    {
        indices_->push_back(i);
    }
};

QuickRandomRollout::~QuickRandomRollout()
{
    delete action_list_;
    delete indices_;
}

float QuickRandomRollout::SimulationOnce(Game *b) const
{
    Game *traverse_b = b->Clone();
    Player turn = traverse_b->GetPlayerThisTurn();
    while (!traverse_b->IsGameOver())
    {
        Action *move = GetRandomMove(b);
        traverse_b->DoAction(move);
    }
    return EvaluateResult(traverse_b, turn);
}

Action *QuickRandomRollout::GetRandomMove(Game *b) const
{
    std::random_shuffle(indices_->begin(), indices_->end());

    for (int i = 0; i < indices_->size(); i++)
    {
        Action *action = action_list_->Get(indices_->at(i));
        if (b->IsMovable(action))
            return action;
    }

    throw std::invalid_argument("QuickRandomRollout::GetRandomMove, No valid actions are found");
}
