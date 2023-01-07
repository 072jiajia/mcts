#include "timecontrol.h"

Timer::Timer()
{
    struct timespec tick;
    clock_gettime(CLOCK_REALTIME, &tick);
    start_time_ = tick.tv_sec + tick.tv_nsec * 1e-9;
}

Timer::~Timer() {}

double Timer::get_duration() const
{
    struct timespec tock;
    clock_gettime(CLOCK_REALTIME, &tock);
    return double(tock.tv_sec + tock.tv_nsec * 1e-9) - start_time_;
}

CountDown::CountDown(double time_limit_ms)
    : timer_(), time_limit_ms_(time_limit_ms) {}

CountDown::~CountDown() {}

bool CountDown::Stop() const
{
    return timer_.get_duration() >= time_limit_ms_ / 1000.;
}

SignalStopper::SignalStopper(bool *stop) : stop_(stop) {}

SignalStopper::~SignalStopper() {}

bool SignalStopper::Stop() const
{
    return *stop_;
}
