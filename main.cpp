#include "src/mcts.h"
#include "games/games.h"
#include <iostream>
#include <vector>
#include <stdexcept>

int main()
{

#ifdef NOGO
    using Game = NoGo;
    Game b(4, 4);
#elif TICTACTOE
    using Game = TicTacToe;
    Game b;
#else
    using Game = TicTacToe;
    Game b;
#endif

    MCTSPlayer<Game> p(1000., 1000);

    while (!b.IsGameOver())
    {
        b.PrintState();
        auto m = p.SearchAction(&b);
        b.DoAction(m);
    }

    b.PrintState();
}