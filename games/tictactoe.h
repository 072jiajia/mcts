#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "game_base.h"

class TicTacToe
{
public:
    GAME_MODULE(TicTacToe);
    struct Action
    {
        int x;
        int y;
        Action(int x_, int y_) : x(x_), y(y_) {}
        Action(const Action &) = default;
    };

    TicTacToe();
    TicTacToe(TicTacToe *);
    ~TicTacToe();

private:
    enum class PieceType
    {
        EMPTY = 0,
        PLAYER1 = 1,
        PLAYER2 = 2,
        UNKNOWN = 3,
    };

    void switch_turn();

    PieceType board[3][3];
    Player whos_turn;
};
