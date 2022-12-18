#pragma once
#include <algorithm>
#include <cstring>
#include <time.h>
#include "../games/game_base.h"

class Timer
{
public:
	Timer();
	virtual ~Timer();
	void reset();
	double get_duration();

private:
	struct timespec tick_;
};

float EvaluateResult(Game *b, Player turn);
