#pragma once

template <typename G>
class MCTSNodeBase;

template <typename G>
class SelectionStrategy
{
public:
    virtual ~SelectionStrategy() = default;
    virtual int Select(MCTSNodeBase<G> *node) const = 0;
};

template <typename G>
class UCBHighest : public SelectionStrategy<G>
{
public:
    UCBHighest(float C = 1.4) : C_(C){};

    int Select(MCTSNodeBase<G> *node) const override
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

private:
    float C_;
};
