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
    virtual ~Action() {}

    virtual int encoding() const
    {
        throw std::invalid_argument("virtual int YourGameAction::encoding() not implemented");
    }

    virtual bool IsSame(const Action *) const
    {
        throw std::invalid_argument("virtual bool YourGameAction::IsSame() not implemented");
    }
};

class ActionList
{
public:
    ActionList();
    ~ActionList();

    void Add(Action *action);
    int GetSize() const;
    Action *Pop(int index);
    Action *Get(int index) const;

private:
    std::vector<Action *> actions_;
};

class Game
{
public:
    virtual ~Game();
    virtual Game *Clone() const = 0;
    virtual ActionList *GetLegalMoves() const = 0;
    virtual void DoAction(const Action *) = 0;
    virtual Player GetPlayerThisTurn() const = 0;
    virtual ResultType GetResult() const = 0;
    virtual bool IsGameOver() const = 0;
    virtual void PrintState() const = 0;
    virtual bool IsMovable(const Action *) const { throw std::logic_error("bool Game::IsMovable not yet implemented"); }
    virtual bool IsSame(const Game *) const { throw std::logic_error("bool Game::IsMovable not yet implemented"); }
};
