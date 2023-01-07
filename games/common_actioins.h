#pragma once

#include "../mcts/game_base.h"

class OneDimensionalAction : public Action
{
public:
    OneDimensionalAction(int position) : position_(position) {}
    OneDimensionalAction(const OneDimensionalAction &) = default;

    int encoding() const { return position_; }
    inline int position() { return position_; }

    bool IsSame(const Action *input_action) const
    {
        OneDimensionalAction *action = (OneDimensionalAction *)input_action;
        if (this->position_ != action->position())
            return false;
        return true;
    }

private:
    int position_;
};

class TwoDimensionalAction : public Action
{
public:
    TwoDimensionalAction(int x, int y) : x_(x), y_(y) {}
    TwoDimensionalAction(const TwoDimensionalAction &) = default;

    int encoding() const { return (x_ << 16) + y_; }
    inline int x() { return x_; }
    inline int y() { return y_; }

    bool IsSame(const Action *input_action) const
    {
        TwoDimensionalAction *action = (TwoDimensionalAction *)input_action;
        if (this->x_ != action->x())
            return false;
        if (this->y_ != action->y())
            return false;
        return true;
    }

private:
    int x_;
    int y_;
};
