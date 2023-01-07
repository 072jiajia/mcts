#pragma once
#include <random>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>

class Timer
{
public:
    Timer();
    ~Timer();
    double get_duration() const;

private:
    double start_time_;
};

class TimeControlStrategy
{
public:
    virtual ~TimeControlStrategy() = default;
    virtual bool Stop() const = 0;
};

class CountDown : public TimeControlStrategy
{
public:
    CountDown(double time_limit_ms);
    ~CountDown();
    bool Stop() const;

private:
    Timer timer_;
    double time_limit_ms_;
};

class SignalStopper : public TimeControlStrategy
{
public:
    SignalStopper(bool *stop);
    ~SignalStopper();
    bool Stop() const;

private:
    bool *stop_;
};
