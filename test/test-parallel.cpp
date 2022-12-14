#include "../mcts/mcts.h"
#include "../games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    /* Use MCTS with root parallel with 30 processes */
    AgentOptions p1_options = AgentOptions(Algorithm::MCTS)
                                  .selection_strategy(new UCBHighest(1.4))
                                  .decision_strategy(new HighestValue())
                                  .time_limit_ms(1000.)
                                  .num_threads(4)
                                  .num_processes(10);

    /* Use MCTS with tree parallel with 4 threads */
    // AgentOptions p1_options = AgentOptions(Algorithm::MCTS_VIRTUAL_LOSS)
    //                               .selection_strategy(new UCBHighestVirtualLoss(1.4))
    //                               .num_threads(4);

    /* Use MCTS with tree parallel with 4 threads
       & also with root parallel with 30 processes */
    // AgentOptions p1_options = AgentOptions(Algorithm::MCTS_VIRTUAL_LOSS)
    //                               .selection_strategy(new UCBHighestVirtualLoss(1.4))
    //                               .time_limit_ms(1500.)
    //                               .num_threads(4)
    //                               .num_processes(30);

    AgentOptions p2_options = AgentOptions(Algorithm::MCTS).selection_strategy(new UCBHighest(1.4));

    Agent p1(p1_options);
    Agent p2(p2_options);

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