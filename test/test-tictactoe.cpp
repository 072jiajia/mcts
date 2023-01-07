#include "../mcts/mcts.h"
#include "../games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    TicTacToe state;

    AgentOptions p_options;
    p_options.search_strategy(new MCTSNodeSearcher());

    Agent p(p_options);

    while (!state.IsGameOver())
    {
        std::cout << state << std::endl;

        Action *action = p.SearchAction(&state);
        state.DoAction(action);
        delete action;
    }

    std::cout << state << std::endl;
}