#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    TicTacToe b;

    Agent p(Agent::Algo::MCTS_COPY_STATE, 1000., 1000);

    while (!b.IsGameOver())
    {
        b.PrintState();

        Action *action = p.SearchAction(&b);
        b.DoAction(action);
        delete action;
    }

    b.PrintState();
}