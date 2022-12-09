#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using Game = TicTacToe;

int main()
{
    Game b;

    MCTSAgent<Game> p(1000., 1000);

    while (!b.IsGameOver())
    {
        b.PrintState();
        auto m = p.SearchAction(&b);
        b.DoAction(m);
    }

    b.PrintState();
}