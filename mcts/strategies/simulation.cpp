#include "simulation.h"

float SimulationDefaultStrategy::SimulationOnce(Game *state) const
{
    Game *traverse_state = state->Clone();
    Player turn = traverse_state->GetPlayerThisTurn();
    while (!traverse_state->IsGameOver())
    {
        Action *move = this->GetRandomMove(traverse_state);
        traverse_state->DoAction(move);
        delete move;
    }
    float output = EvaluateResult(traverse_state, turn);
    delete traverse_state;
    return output;
}

Action *SimulationDefaultStrategy::GetRandomMove(Game *state) const
{
    ActionList *movable_actions = state->GetLegalMoves();
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
    LeadParallelInput *args = (LeadParallelInput *)void_args;
    SimulationStrategy *strategy = args->strategy();
    Game *state = args->state();
    float *result_ptr = args->result_ptr();

    *result_ptr = strategy->SimulationOnce(state);

    delete args;
    pthread_exit(NULL);
}

float ParallelSimulationStrategy::SimulationOnce(Game *state) const
{

    for (int i = 0; i < num_threads_; i++)
    {
        LeadParallelInput *data = new LeadParallelInput(strategy_, state, &(results[i]));
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

QuickRandomRollout::QuickRandomRollout(ActionList *action_space)
    : action_space_(action_space), indices_(new std::vector<int>())
{
    for (int i = 0; i < action_space->GetSize(); i++)
    {
        indices_->push_back(i);
    }
};

QuickRandomRollout::~QuickRandomRollout()
{
    delete action_space_;
    delete indices_;
}

float QuickRandomRollout::SimulationOnce(Game *state) const
{
    Game *traverse_state = state->Clone();
    Player turn = traverse_state->GetPlayerThisTurn();
    while (!traverse_state->IsGameOver())
    {
        Action *move = GetRandomMove(state);
        traverse_state->DoAction(move);
    }
    return EvaluateResult(traverse_state, turn);
}

Action *QuickRandomRollout::GetRandomMove(Game *state) const
{
    std::random_shuffle(indices_->begin(), indices_->end());

    for (int i = 0; i < indices_->size(); i++)
    {
        Action *action = action_space_->Get(indices_->at(i));
        if (state->IsMovable(action))
            return action;
    }

    throw std::invalid_argument("QuickRandomRollout::GetRandomMove, No valid actions are found");
}
