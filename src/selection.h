#pragma once
#include "utils.h"

class MCTSNode_;
class RaveNode;

class SelectionStrategy
{
public:
    virtual ~SelectionStrategy() = default;
    virtual int Select(MCTSNode_ *node) const = 0;
};

class UCBHighest : public SelectionStrategy
{
public:
    UCBHighest(float C = 1.4);

    int Select(MCTSNode_ *node) const;

private:
    float C_;
};

class RaveUCBHighest : public SelectionStrategy
{
public:
    RaveUCBHighest(float C = 1.4);

    int Select(MCTSNode_ *node) const;

private:
    float C_;
};
