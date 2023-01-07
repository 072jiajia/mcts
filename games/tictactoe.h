#pragma once
#include <vector>
#include <stdexcept>
#include <iostream>

#include "common_actioins.h"
#include "../mcts/game_base.h"

using TicTacToeAction = TwoDimensionalAction;

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

    bool IsSame(const Game *) const;

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
