#pragma once

#include <vector>
#include <stdexcept>

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
public:
    virtual int encoding();
};

class ActionList
{
public:
    ActionList();
    ~ActionList();

    void Add(Action *action);
    int GetSize();
    Action *Pop(int index);
    Action *Get(int index) const;

private:
    std::vector<Action *> actions_;
};

class Game
{
public:
    virtual Game *Clone() = 0;
    virtual ActionList *GetLegalMoves() = 0;
    virtual void DoAction(const Action *) = 0;
    virtual Player GetPlayerThisTurn() = 0;
    virtual ResultType GetResult() = 0;
    virtual bool IsGameOver() = 0;
    virtual void PrintState() = 0;
};
