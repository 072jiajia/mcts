#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

using Game = NoGo;

int main()
{
    Game b(4, 4);

    // MCTSAgent<Game> p(1000., 1000);
    // MCTSAgent<Game> p(1000., 1000, new SimulationDefaultStrategy<Game>());
    MCTSAgent<Game> p1(1000., 1000, new UCBHighest<Game>(0.7), new QuickRandomRollout<Game>(b.GetActionSpace()));
    MCTSAgent<Game> p2(1000., 1000, new UCBHighest<Game>(1.4), new QuickRandomRollout<Game>(b.GetActionSpace()));

    while (!b.IsGameOver())
    {
        b.PrintState();

        MCTSAgent<Game> *current_player;
        if (b.GetPlayerThisTurn() == Game::Player::PLAYER1)
            current_player = &p1;
        else
            current_player = &p2;

        auto m = current_player->SearchAction(&b);
        b.DoAction(m);
    }

    b.PrintState();
}