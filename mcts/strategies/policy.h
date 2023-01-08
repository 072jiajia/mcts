#pragma once
#include "../utils.h"

class MCTSNode_;

class PolicyStrategy
{
public:
    virtual ~PolicyStrategy() = default;
    virtual std::vector<float> *Estimate(const MCTSNode_ *, const Game *, const ActionList *) const = 0;
};

class DefaultPolicy : public PolicyStrategy
{
public:
    DefaultPolicy();
    std::vector<float> *Estimate(const MCTSNode_ *, const Game *, const ActionList *) const;
};
