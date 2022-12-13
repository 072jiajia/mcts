#include "node.h"
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
