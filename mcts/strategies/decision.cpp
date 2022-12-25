#include "decision.h"
#include "../tree.h"

MostFrequency::~MostFrequency() {}

int MostFrequency::Choose(MCTSTree_ *tree) const
{
    std::vector<int> freq = tree->GetFrequencies();
    int best_move = -1;
    int best_value = -1;
    for (int i = 0; i < freq.size(); i++)
    {
        if (freq[i] > best_value)
        {
            best_value = freq[i];
            best_move = i;
        }
    }
    return best_move;
}
