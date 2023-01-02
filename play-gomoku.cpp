#include "mcts/mcts.h"
#include "games/games.h"
#include "mcts/strategies/misc/gomoku_strategy.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    AgentOptions ai_args = AgentOptions(Algorithm::MCTS_PUCT)
                               .selection_strategy(new PUCT(1.4))
                               .policy_strategy(new GomokuCenterFirst());
    Agent ai(ai_args);

    Gomoku b;
    while (!b.IsGameOver())
    {
        b.PrintState();
        Action *action;
        Agent *current_player;
        if (b.GetPlayerThisTurn() == Player::PLAYER1)
        {
            int x;
            char y;
            std::cout << "Your turn! Please input your move (ex: I8, A15): ";
            std::cin >> y >> x;
            action = new GomokuAction(x - 1, y - 'A');
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