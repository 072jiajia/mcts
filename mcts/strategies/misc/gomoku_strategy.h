#pragma once
#include "../policy.h"
#include "../simulation.h"
#include "../../../games/games.h"

class GomokuCenterFirst : public PolicyStrategy
{
public:
    GomokuCenterFirst();
    std::vector<float> *Estimate(const MCTSNode_ *, const Game *, const ActionList *) const;
};
