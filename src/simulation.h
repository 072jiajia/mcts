
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