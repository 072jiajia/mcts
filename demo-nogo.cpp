#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{
    NoGo b(4, 4);

    // MCTSAgent p(1000., 1000);
    // MCTSAgent p(1000., 1000, new SimulationDefaultStrategy());
    // MCTSAgent p1(1000., 1000, new UCBHighest(0.7), new QuickRandomRollout(b.GetActionSpace()));
    // MCTSAgent p2(1000., 1000, new UCBHighest(1.4), new QuickRandomRollout(b.GetActionSpace()));
    MCTSAgent p1(1000., 1000, new UCBHighest(0.7));
    MCTSAgent p2(1000., 1000, new UCBHighest(1.4));

    while (!b.IsGameOver())
    {
        b.PrintState();

        MCTSAgent *current_player;
        if (b.GetPlayerThisTurn() == Player::PLAYER1)
            current_player = &p1;
        else
            current_player = &p2;

        Action *action = current_player->SearchAction(&b);
        b.DoAction(action);
        delete action;
    }

    b.PrintState();
}