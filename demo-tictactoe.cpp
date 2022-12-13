#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    TicTacToe b;

    MCTSAgent p(1000., 1000);

    while (!b.IsGameOver())
    {
        b.PrintState();
        auto m = p.SearchAction(&b);
        b.DoAction(m);
    }

    b.PrintState();
}