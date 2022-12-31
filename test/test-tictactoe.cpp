#include "../mcts/mcts.h"
#include "../games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    TicTacToe b;

    AgentOptions p_options(Algorithm::MCTS);
    Agent p(p_options);

    while (!b.IsGameOver())
    {
        b.PrintState();

        Action *action = p.SearchAction(&b);
        b.DoAction(action);
        delete action;
    }

    b.PrintState();
}