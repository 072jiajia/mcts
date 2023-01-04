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

    ConnectFour b;
    while (!b.IsGameOver())
    {
        b.PrintState();
        Action *action;
        Agent *current_player;
        if (b.GetPlayerThisTurn() == Player::PLAYER1)
        {
            int x;
            std::cout << "Your turn! Please input your move (1~7): ";
            std::cin >> x;
            action = new ConnectFourAction(x - 1, -1);
        }
        else
        {
            action = ai.SearchAction(&b);
        }
        b.DoAction(action);
        delete action;
    }
    b.PrintState();
    if (b.GetResult() == ResultType::PLAYER1_WIN)
        std::cout << "player1 wins" << std::endl;
    else if (b.GetResult() == ResultType::PLAYER2_WIN)
        std::cout << "player2 wins" << std::endl;
    else if (b.GetResult() == ResultType::DRAW)
        std::cout << "Draw" << std::endl;
}