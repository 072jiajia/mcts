#pragma once
#include <array>
#include <list>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <cmath>

#include "common_actioins.h"
#include "../mcts/game_base.h"

using NoGoAction = TwoDimensionalAction;

class NoGo : public Game
{
public:
    NoGo(int size_x, int size_y);
    NoGo(const NoGo *src);
    ~NoGo();

    NoGo *Clone() const;
    ActionList *GetLegalMoves() const;
    void DoAction(const Action *);
    Player GetPlayerThisTurn() const;
    ResultType GetResult() const;
    bool IsGameOver() const;
    void PrintState() const;
    bool IsMovable(const Action *) const;

    static ActionList *GetActionSpace(int size_x, int size_y);

    bool IsSame(const Game *) const;

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
