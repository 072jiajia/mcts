#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "../mcts/game_base.h"

class GomokuAction : public Action
{
public:
    GomokuAction(int x, int y) : x_(x), y_(y) {}
    GomokuAction(const GomokuAction &) = default;

    int encoding() const { return (x_ << 16) + y_; }
    inline int x() { return x_; }
    inline int y() { return y_; }

private:
    int x_;
    int y_;
};

class Gomoku : public Game
{
public:
    enum class PieceType
    {
        EMPTY = 0,
        PLAYER1 = 1,
        PLAYER2 = 2,
        UNKNOWN = 3,
    };

    Gomoku();
    Gomoku(const Gomoku *);
    ~Gomoku();

    Gomoku *Clone() const;
    ActionList *GetLegalMoves() const;
    void DoAction(const Action *);
    Player GetPlayerThisTurn() const;
    ResultType GetResult() const;
    bool IsGameOver() const;
    void PrintState() const;

    PieceType *GetBoard();
    bool IsSame(Game *) const;

private:
    int LineConnected(int x, int y, int dx, int dy) const;
    void switch_turn();

    int remaining_space_;
    PieceType board[225];
    Player whos_turn;
    ResultType result_;
};
