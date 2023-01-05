#include "../mcts/mcts.h"
#include "../games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    /* Use MCTS with root parallel with 30 processes */
    AgentOptions p1_options = AgentOptions()
                                  .search_strategy(new MCTSNodeSearcher())
                                  .time_limit_ms(1000.)
                                  .num_threads(4)
                                  .num_processes(10);

    AgentOptions p2_options = AgentOptions()
                                  .search_strategy(new MCTSNodeSearcher());

    Agent p1(p1_options);
    Agent p2(p2_options);

    int p1_win = 0;
    int p2_win = 0;
    int draw_count = 0;
    while (true)
    {
        NoGo state(5, 5);
        while (!state.IsGameOver())
        {
            state.PrintState();
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
        state.PrintState();
        if (state.GetResult() == ResultType::PLAYER1_WIN)
            p1_win++;
        else if (state.GetResult() == ResultType::PLAYER2_WIN)
            p2_win++;
        else if (state.GetResult() == ResultType::DRAW)
            draw_count++;

        std::cout << p1_win << " / " << draw_count << " / " << p2_win << std::endl;
    }
}