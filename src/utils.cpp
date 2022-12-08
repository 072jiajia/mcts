#include "utils.h"

Timer::Timer() {}
Timer::~Timer() {}

void Timer::reset()
{
    clock_gettime(CLOCK_REALTIME, &tick_);
}

double Timer::get_duration()
{
    clock_gettime(CLOCK_REALTIME, &tock_);
    return (double)((tock_.tv_sec + tock_.tv_nsec * 1e-9) - (double)(tick_.tv_sec + tick_.tv_nsec * 1e-9));
}
