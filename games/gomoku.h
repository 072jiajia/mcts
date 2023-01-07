#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "common_actioins.h"
#include "../mcts/game_base.h"

using GomokuAction = TwoDimensionalAction;

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
    void PrintState(std::ostream &out) const;

    PieceType *GetBoard();
    bool IsSame(const Game *) const;

private:
    int LineConnected(int x, int y, int dx, int dy) const;
    void switch_turn();

    int remaining_space_;
    PieceType board[225];
    Player whos_turn;
    ResultType result_;
};
