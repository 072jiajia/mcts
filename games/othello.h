#pragma once
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>

#include "common_actioins.h"
#include "../mcts/game_base.h"

using OthelloAction = OneDimensionalAction;

class Othello : public Game
{
public:
    Othello();
    Othello(const Othello *state);
    ~Othello();

    Othello *Clone() const;

    ActionList *GetLegalMoves() const;
    void DoAction(const Action *action);
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

    bool canFlip(int8_t i, int8_t j, int8_t di, int8_t dj) const;
    void doFlip(int8_t i, int8_t j, int8_t di, int8_t dj);

    PieceType board_[64];
    Player whos_turn_;
    bool last_skipped_;
    bool gameover_;
};
