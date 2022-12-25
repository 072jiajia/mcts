#pragma once
#include <random>
#include <semaphore.h>
#include <pthread.h>
#include <iostream>

#include "../utils.h"

class MCTSTree_;

class DecisionStrategy
{
public:
    virtual ~DecisionStrategy() = default;
    virtual int Choose(MCTSTree_ *) const = 0;
};

class MostFrequency : public DecisionStrategy
{
public:
    ~MostFrequency();
    int Choose(MCTSTree_ *) const;
};
