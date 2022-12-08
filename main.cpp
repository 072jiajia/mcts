#include "src/mcts.h"
#include "games/tictactoe.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using Game = TicTacToe;

int main()
{
    Game b;

    MCTSPlayer<Game> p;

    while (!b.IsGameOver())
    {
        b.PrintState();
        auto m = p.SearchAction(&b);
        b.DoAction(m);
    }

    b.PrintState();
}