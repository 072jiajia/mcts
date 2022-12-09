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

class NoGo
{
public:
    GAME_MODULE(NoGo);
    struct Action
    {
        int x;
        int y;
        Action(int x_, int y_) : x(x_), y(y_) {}
        Action(const Action &) = default;
    };

    NoGo(int size_x, int size_y);
    NoGo(NoGo *src);
    ~NoGo();

    NoGo *Clone();
    std::vector<Action> GetLegalMoves();
    void DoAction(const Action action);

    Player GetPlayerThisTurn();
    ResultType GetResult();
    bool IsGameOver();

    void PrintState();

public:
    std::vector<Action> *GetActionSpace();
    static bool IsMovable(NoGo &state, const Action &action);

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
