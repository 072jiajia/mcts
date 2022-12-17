#pragma once
#include <array>
#include <list>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <cmath>
#include <vector>
#include <map>
#include "game_base.h"

class NoGoAction : public Action
{
public:
    NoGoAction(int x, int y) : x_(x), y_(y) {}
    NoGoAction(const NoGoAction &) = default;
    int encoding() { return (x_ << 16) + y_; }

    inline int x() { return x_; }
    inline int y() { return y_; }

private:
    int x_;
    int y_;
};

class NoGo : public Game
{
public:
    NoGo(int size_x, int size_y);
    NoGo(NoGo *src);
    virtual ~NoGo();

    NoGo *Clone();
    ActionList *GetLegalMoves();
    void DoAction(const Action *);
    Player GetPlayerThisTurn();
    ResultType GetResult();
    bool IsGameOver();
    void PrintState();

    ActionList *GetActionSpace();
    static bool IsMovable(NoGo &state, const Action *action);

private:
    enum class PieceType
    {
        EMPTY = 0,
        PLAYER1 = 1,
        PLAYER2 = 2,
        UNKNOWN = 3,
    };

    struct Position
    {
        int x;
        int y;
        Position(int x_, int y_) : x(x_), y(y_) {}
    };

    PieceType GetPlayersPiece(Player);
    void switch_turn();
    static bool DoesHaveLiberty(NoGo &state, int x, int y, PieceType who);

    int size_x_;
    int size_y_;
    Player whos_turn_;
    PieceType *flat_board_;
};
