#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "../games/game_base.h"

class Timer
{
public:
	Timer();
	~Timer();
	void reset();
	double get_duration();

private:
	struct timespec tick_, tock_;
};

float EvaluateResult(Game *b, Player turn);
