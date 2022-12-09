#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using Game = NoGo;

int main()
{
    Game b(4, 4);

    // MCTSPlayer<Game> p(1000., 1000);
    // MCTSPlayer<Game> p(1000., 1000, new SimulationDefaultStrategy<Game>());
    MCTSPlayer<Game> p(1000., 1000, new QuickRandomRollout<Game>(b.GetActionSpace()));

    while (!b.IsGameOver())
    {
        b.PrintState();
        auto m = p.SearchAction(&b);
        b.DoAction(m);
    }

    b.PrintState();
}