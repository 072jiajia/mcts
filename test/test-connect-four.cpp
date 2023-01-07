#include "../mcts/mcts.h"
#include "../games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    AgentOptions p1_args = AgentOptions()
                               .search_strategy(new MCTSNodeSearcher())
                               .num_processes(8);
    AgentOptions p2_args = AgentOptions()
                               .search_strategy(new MCTSNodeSearcher());
    Agent p1(p1_args);
    Agent p2(p2_args);

    int p1_win = 0;
    int p2_win = 0;
    int draw_count = 0;
    while (true)
    {
        ConnectFour state;
        while (!state.IsGameOver())
        {
            std::cout << state << std::endl;
            Action *action;
            Agent *current_player;
            if (state.GetPlayerThisTurn() == Player::PLAYER1)
            {
                current_player = &p1;
                action = current_player->SearchAction(&state);
            }
            else
            {
                current_player = &p2;
                action = current_player->SearchAction(&state);
            }
            state.DoAction(action);
            delete action;
        }
        std::cout << state << std::endl;
        if (state.GetResult() == ResultType::PLAYER1_WIN)
            p1_win++;
        else if (state.GetResult() == ResultType::PLAYER2_WIN)
            p2_win++;
        else if (state.GetResult() == ResultType::DRAW)
            draw_count++;

        std::cout << p1_win << " / " << draw_count << " / " << p2_win << std::endl;
    }
}