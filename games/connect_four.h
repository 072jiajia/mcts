#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "../mcts/game_base.h"

class ConnectFourAction : public Action
{
public:
    ConnectFourAction(int x, int y) : x_(x), y_(y) {}
    ConnectFourAction(const ConnectFourAction &) = default;

    int encoding() const { return (x_ << 16) + y_; }
    inline int x() { return x_; }
    inline int y() { return y_; }

private:
    int x_;
    int y_;
};

class ConnectFour : public Game
{
public:
    enum class PieceType
    {
        EMPTY = 0,
        PLAYER1 = 1,
        PLAYER2 = 2,
        UNKNOWN = 3,
    };

    ConnectFour();
    ConnectFour(const ConnectFour *);
    ~ConnectFour();

    ConnectFour *Clone() const;
    ActionList *GetLegalMoves() const;
    void DoAction(const Action *);
    Player GetPlayerThisTurn() const;
    ResultType GetResult() const;
    bool IsGameOver() const;
    void PrintState() const;

    PieceType *GetBoard();

private:
    int LineConnected(int x, int y, int dx, int dy) const;
    void switch_turn();

    int remaining_space_;
    PieceType board_[7][6];
    int line_height_[7];
    Player whos_turn_;
    ResultType result_;
};
