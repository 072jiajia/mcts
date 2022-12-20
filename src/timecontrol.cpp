#include "timecontrol.h"

CountDown::CountDown(Timer *timer, double time_limit_ms)
    : timer_(timer), time_limit_ms_(time_limit_ms) {}

CountDown::~CountDown() {}

bool CountDown::Stop() const
{
    return timer_->get_duration() >= time_limit_ms_ / 1000.;
}
