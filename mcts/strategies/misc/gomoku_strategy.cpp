#include "gomoku_strategy.h"

GomokuCenterFirst::GomokuCenterFirst() {}

std::vector<float> *GomokuCenterFirst::Estimate(const MCTSNode_ *node, const Game *input_state, const ActionList *action_list) const
{
    int action_size = action_list->GetSize();
    std::vector<float> *output = new std::vector<float>(action_size);

    // Get logits by offsets from the center (7, 7);
    for (int i = 0; i < action_size; i++)
    {
        GomokuAction *action = (GomokuAction *)(action_list->Get(i));
        float offset = std::abs(action->x() - 7) + std::abs(action->y() - 7);
        output->at(i) = -offset;
    }

    /* Normalize the logits to prevent very large
     * or very small number to compute std::exp()
     */
    float max_logit = -1e8;
    for (int i = 0; i < action_size; i++)
    {
        max_logit = std::max(output->at(i), max_logit);
    }

    float total = 0;
    for (int i = 0; i < action_size; i++)
    {
        output->at(i) = std::exp(output->at(i) - max_logit);
        total += output->at(i);
    }

    /* Calculate the output */
    for (int i = 0; i < action_size; i++)
    {
        output->at(i) /= total;
    }

    return output;
}
