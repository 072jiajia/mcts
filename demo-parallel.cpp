#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    // Agent p1(Agent::Algo::MCTS_LEAF_PARALLEL, 1000., 1000, new UCBHighest(1.4), new ParallelSimulationStrategy(4, new SimulationDefaultStrategy()));
    Agent p1(Agent::Algo::MCTS_ROOT_PARALLEL, 1000., 1000, new UCBHighest(1.4), nullptr, 16);
    Agent p2(Agent::Algo::MCTS, 1000., 1000, new UCBHighest(1.4));

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