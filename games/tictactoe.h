#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "../mcts/game_base.h"

class TicTacToeAction : public Action
{
public:
    TicTacToeAction(int x, int y) : x_(x), y_(y) {}
    TicTacToeAction(const TicTacToeAction &) = default;

    int encoding() const { return (x_ << 16) + y_; }
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
    TicTacToe(const TicTacToe *);
    ~TicTacToe();

    TicTacToe *Clone() const;
    ActionList *GetLegalMoves() const;
    void DoAction(const Action *);
    Player GetPlayerThisTurn() const;
    ResultType GetResult() const;
    bool IsGameOver() const;
    void PrintState() const;

    bool IsSame(Game *) const;

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
