#include "../node.h"
#include "../utils.h"

UCBHighest::UCBHighest(float C) : C_(C){};

int UCBHighest::Select(MCTSNode_ *node) const
{

    const std::vector<MCTSNode_ *> *children = node->GetChildren();

    float best_value = -1e20;
    int best_move = -1;
    float logN = std::log(node->N() + 1.);
    for (uint i = 0; i < children->size(); i++)
    {
        float value = -children->at(i)->Q() + this->C_ * std::sqrt(logN / (children->at(i)->N() + 1));

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}

RaveUCBHighest::RaveUCBHighest(float C) : C_(C){};

int RaveUCBHighest::Select(MCTSNode_ *node_abs) const
{
    RaveNode *node = (RaveNode *)node_abs;
    const std::vector<MCTSNode_ *> *children = node->GetChildren();

    float best_value = -1e20;
    int best_move = -1;
    float logN = std::log(node->N() + 1.);
    for (uint i = 0; i < children->size(); i++)
    {
        float child_Q = (-1) * children->at(i)->Q();
        float child_N = children->at(i)->N();
        float rave_Q = node->rave_Q(i);
        float rave_N = node->rave_N(i);
        float beta = rave_N / (child_N + rave_N + 4 * child_N * rave_N * 0.1 + 1e-8);
        float value = (child_Q * (1 - beta) + beta * rave_Q) + this->C_ * std::sqrt(logN / (child_N + 1));

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}

UCBHighestVirtualLoss::UCBHighestVirtualLoss(float C) : C_(C){};

int UCBHighestVirtualLoss::Select(MCTSNode_ *node_abs) const
{
    MCTSMutexNode *node = (MCTSMutexNode *)node_abs;
    const std::vector<MCTSNode_ *> *children = node->GetChildren();

    float best_value = -1e20;
    int best_move = -1;
    float logN = std::log(node->N() + 1.);
    for (uint i = 0; i < children->size(); i++)
    {
        MCTSMutexNode *child = (MCTSMutexNode *)(children->at(i));
        child->Lock();

        float Q = child->Q();
        float N = child->N() + 1e-4;
        float virtual_N = child->GetVirtualN();
        float value = -((Q * N + virtual_N) / (N + virtual_N)) + this->C_ * std::sqrt(logN / (N + virtual_N + 1));
        child->Release();

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }

    return best_move;
}

PUCT::PUCT(float C) : C_(C){};

int PUCT::Select(MCTSNode_ *node) const
{
    const std::vector<MCTSNode_ *> *children = node->GetChildren();
    const std::vector<float> *policy = ((MCTSPolicyNode *)node)->GetPolicy();

    float best_value = -1e20;
    int best_move = -1;
    float CsqrtN = this->C_ * std::sqrt(node->N() + 1.);
    for (uint i = 0; i < children->size(); i++)
    {
        float value = -children->at(i)->Q() + policy->at(i) * CsqrtN / (children->at(i)->N() + 1);

        if (value > best_value)
        {
            best_value = value;
            best_move = i;
        }
    }
    return best_move;
}
