#pragma once
#include <random>
#include "utils.h"
#include <semaphore.h>
#include <pthread.h>
#include <iostream>

class TimeControlStrategy
{
public:
    virtual ~TimeControlStrategy() = default;
    virtual bool Stop() const = 0;
};

class CountDown : public TimeControlStrategy
{
public:
    CountDown(Timer *timer, double time_limit_ms);

    ~CountDown();

    bool Stop() const;

private:
    Timer *timer_;
    double time_limit_ms_;
};
