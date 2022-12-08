#pragma once

#define GAME_MODULE(game_name) \
                               \
public:                        \
    enum class Player          \
    {                          \
        PLAYER1 = 0,           \
        PLAYER2 = 1,           \
    };                         \
                               \
    enum class ResultType      \
    {                          \
        DRAW = 0,              \
        PLAYER1_WIN = 1,       \
        PLAYER2_WIN = 2,       \
        NOT_FINISH_YET = 3,    \
    };
