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

HighestValue::~HighestValue() {}

int HighestValue::Choose(MCTSTree_ *tree) const
{
    std::vector<float> value = tree->GetValues();
    int best_move = -1;
    float best_value = -1e20;
    for (int i = 0; i < value.size(); i++)
    {
        if (-value[i] > best_value)
        {
            best_value = -value[i];
            best_move = i;
        }
    }
    return best_move;
}
