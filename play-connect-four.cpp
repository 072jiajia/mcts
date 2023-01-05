#include "mcts/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    AgentOptions ai_args = AgentOptions()
                               .search_strategy(new MCTSNodeSearcher());
    Agent ai(ai_args);

    ConnectFour state;
    while (!state.IsGameOver())
    {
        state.PrintState();
        Action *action;
        Agent *current_player;
        if (state.GetPlayerThisTurn() == Player::PLAYER1)
        {
            int x;
            std::cout << "Your turn! Please input your move (1~7): ";
            std::cin >> x;
            action = new ConnectFourAction(x - 1, -1);
        }
        else
        {
            action = ai.SearchAction(&state);
        }
        state.DoAction(action);
        delete action;
    }
    state.PrintState();
    if (state.GetResult() == ResultType::PLAYER1_WIN)
        std::cout << "player1 wins" << std::endl;
    else if (state.GetResult() == ResultType::PLAYER2_WIN)
        std::cout << "player2 wins" << std::endl;
    else if (state.GetResult() == ResultType::DRAW)
        std::cout << "Draw" << std::endl;
}