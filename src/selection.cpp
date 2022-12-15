#include "node.h"
#include "rave.h"
#include "utils.h"

UCBHighest::UCBHighest(float C) : C_(C){};

int UCBHighest::Select(MCTSNodeBase *node) const
{
    const auto &children = node->GetChildren();

    float best_value = -1e20;
    int best_move = -1;
    float logN = std::log(node->N() + 1.);
    for (uint i = 0; i < children.size(); i++)
    {
        float value = -children[i]->Q() + this->C_ * std::sqrt(logN / (children[i]->N() + 1));

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}

RaveUCBHighest::RaveUCBHighest(float C) : C_(C){};

int RaveUCBHighest::Select(MCTSNodeBase *node_abs) const
{
    RaveNode *node = (RaveNode *)node_abs;
    const auto &children = node->GetChildren();

    float best_value = -1e20;
    int best_move = -1;
    float logN = std::log(node->N() + 1.);
    for (uint i = 0; i < children.size(); i++)
    {
        float child_Q = (-1) * children[i]->Q();
        float child_N = children[i]->N();
        float rave_Q = node->rave_Q(i);
        float rave_N = node->rave_N(i);
        float beta = rave_N / (child_N + rave_N + 4 * child_N * rave_N * 0.025 + 1e-8);
        float value = (child_Q * (1 - beta) + beta * rave_Q) + this->C_ * std::sqrt(logN / (child_N + 1));

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}
