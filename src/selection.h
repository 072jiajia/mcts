#pragma once
#include "utils.h"

class MCTSNodeBase;
class RaveNode;

class SelectionStrategy
{
public:
    virtual ~SelectionStrategy() = default;
    virtual int Select(MCTSNodeBase *node) const = 0;
};

class UCBHighest : public SelectionStrategy
{
public:
    UCBHighest(float C = 1.4);

    int Select(MCTSNodeBase *node) const;

private:
    float C_;
};

class RaveUCBHighest : public SelectionStrategy
{
public:
    RaveUCBHighest(float C = 1.4);

    int Select(MCTSNodeBase *node) const;

private:
    float C_;
};
