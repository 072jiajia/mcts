#include "../mcts/mcts.h"
#include "../games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    AgentOptions p1_args = AgentOptions()
                               .search_strategy(new MCTSNodeSearcher(nullptr, new QuickRandomRollout(NoGo::GetActionSpace(5, 5))));
    AgentOptions p2_args = AgentOptions()
                               .search_strategy(new MCTSNodeSearcher());
    Agent p1(p1_args);
    Agent p2(p2_args);

    int p1_win = 0;
    int p2_win = 0;
    int draw_count = 0;
    while (true)
    {
        NoGo b(5, 5);
        while (!b.IsGameOver())
        {
            b.PrintState();
            Action *action;
            Agent *current_player;
            if (b.GetPlayerThisTurn() == Player::PLAYER1)
            {
                current_player = &p1;
                action = current_player->SearchAction(&b);
            }
            else
            {
                current_player = &p2;
                action = current_player->SearchAction(&b);
            }
            b.DoAction(action);
            delete action;
        }
        b.PrintState();
        if (b.GetResult() == ResultType::PLAYER1_WIN)
            p1_win++;
        else if (b.GetResult() == ResultType::PLAYER2_WIN)
            p2_win++;
        else if (b.GetResult() == ResultType::DRAW)
            draw_count++;

        std::cout << p1_win << " / " << draw_count << " / " << p2_win << std::endl;
    }
}