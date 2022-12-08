#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>

class Timer
{
private:
	struct timespec tick_, tock_;

public:
	Timer();
	~Timer();
	void reset();
	double get_duration();
};

template <typename G>
float EvaluateResult(G *b, typename G::Player turn)
{
	typename G::ResultType result = b->GetResult();
	if (result == G::ResultType::DRAW)
		return 0.;
	if (result == G::ResultType::PLAYER1_WIN)
		return turn == G::Player::PLAYER1 ? 1. : -1.;
	if (result == G::ResultType::PLAYER2_WIN)
		return turn == G::Player::PLAYER2 ? 1. : -1.;
}
