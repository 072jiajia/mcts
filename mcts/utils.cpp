#include "utils.h"

float EvaluateResult(Game *state, Player turn)
{
    ResultType result = state->GetResult();
    if (result == ResultType::DRAW)
        return 0.;
    if (result == ResultType::PLAYER1_WIN)
        return turn == Player::PLAYER1 ? 1. : -1.;
    if (result == ResultType::PLAYER2_WIN)
        return turn == Player::PLAYER2 ? 1. : -1.;
}
