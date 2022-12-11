#pragma once
#include <random>

template <typename G>
class SimulationStrategy
{
public:
    virtual ~SimulationStrategy() = default;
    virtual float SimulationOnce(G *b) const = 0;
};

template <typename G>
class SimulationDefaultStrategy : public SimulationStrategy<G>
{
public:
    float SimulationOnce(G *b) const override
    {
        G *traverse_b = b->Clone();
        typename G::Player turn = traverse_b->GetPlayerThisTurn();
        while (!traverse_b->IsGameOver())
        {
            int m = this->GetRandomMove(traverse_b);
            auto move = traverse_b->GetLegalMoves()[m];
            traverse_b->DoAction(move);
        }
        return EvaluateResult(traverse_b, turn);
    }

private:
    int GetRandomMove(G *b) const
    {
        auto movable_actions = b->GetLegalMoves();
        int m = rand() % movable_actions.size();
        return m;
    }
};

template <typename G>
class QuickRandomRollout : public SimulationStrategy<G>
{
public:
    QuickRandomRollout(std::vector<typename G::Action> *action_space)
        : action_space_(action_space){};

    ~QuickRandomRollout() { delete action_space_; }

public:
    float SimulationOnce(G *b) const override
    {
        G *traverse_b = b->Clone();
        typename G::Player turn = traverse_b->GetPlayerThisTurn();
        while (!traverse_b->IsGameOver())
        {
            typename G::Action move = GetRandomMove(b);
            traverse_b->DoAction(move);
        }
        return EvaluateResult(traverse_b, turn);
    }

private:
    std::vector<typename G::Action> *action_space_;

    inline typename G::Action GetRandomMove(G *b) const
    {
        std::random_shuffle(action_space_->begin(), action_space_->end());

        for (int i = 0; i < action_space_->size(); i++)
        {
            typename G::Action &action = action_space_->at(i);
            if (G::IsMovable(*b, action))
                return action;
        }

        throw std::invalid_argument("QuickRandomRollout::GetRandomMove, No valid actions are found");
    }
};
