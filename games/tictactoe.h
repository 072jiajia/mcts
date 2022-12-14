#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "game_base.h"

class TicTacToeAction : public Action
{
public:
    TicTacToeAction(int x, int y) : x_(x), y_(y) {}
    TicTacToeAction(const TicTacToeAction &) = default;

    inline int x() { return x_; }
    inline int y() { return y_; }

private:
    int x_;
    int y_;
};

class TicTacToe : public Game
{
public:
    TicTacToe();
    TicTacToe(TicTacToe *);
    ~TicTacToe();

    TicTacToe *Clone();
    ActionList *GetLegalMoves();
    void DoAction(const Action *);
    Player GetPlayerThisTurn();
    ResultType GetResult();
    bool IsGameOver();
    void PrintState();

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
