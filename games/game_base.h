#pragma once

#include <vector>

enum class Player
{
    PLAYER1,
    PLAYER2,
};

enum class ResultType
{
    DRAW,
    PLAYER1_WIN,
    PLAYER2_WIN,
    NOT_FINISH_YET,
};

class Action
{
};

class Game
{
public:
    virtual Game *Clone() = 0;
    virtual std::vector<Action *> GetLegalMoves() = 0;
    virtual void DoAction(const Action *) = 0;
    virtual Player GetPlayerThisTurn() = 0;
    virtual ResultType GetResult() = 0;
    virtual bool IsGameOver() = 0;
    virtual void PrintState() = 0;
};
