#pragma once
#include "../utils.h"

class MCTSNode_;

class PolicyStrategy
{
public:
    virtual ~PolicyStrategy() = default;
    virtual std::vector<float> *Estimate(MCTSNode_ *, Game *, ActionList *) const = 0;
};

class DefaultPolicy : public PolicyStrategy
{
public:
    DefaultPolicy();
    std::vector<float> *Estimate(MCTSNode_ *, Game *, ActionList *) const;
};
