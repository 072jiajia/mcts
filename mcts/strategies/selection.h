#pragma once
#include "../utils.h"

class MCTSNode_;
class RaveNode;
class MCTSMutexNode;
class MCTSPolicyNode;

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
    RaveUCBHighest(float C = 1.4, float beta = 0.1);

    int Select(MCTSNode_ *node) const;

private:
    float C_;
    float beta_;
};

class UCBHighestVirtualLoss : public SelectionStrategy
{
public:
    UCBHighestVirtualLoss(float C = 1.4);

    int Select(MCTSNode_ *node) const;

private:
    float C_;
};

class PUCT : public SelectionStrategy
{
public:
    PUCT(float C = 1.4);

    int Select(MCTSNode_ *node) const;

private:
    float C_;
};
