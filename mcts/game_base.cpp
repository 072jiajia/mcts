#include "game_base.h"

int Action::encoding() const
{
    throw std::invalid_argument("virtual int YourGameAction::encoding() not implemented");
}

Action::~Action() {}

ActionList::ActionList() {}

ActionList::~ActionList()
{
    for (int i = 0; i < actions_.size(); i++)
    {
        if (actions_[i] != nullptr)
        {
            delete actions_[i];
        }
    }
};

void ActionList::Add(Action *action)
{
    actions_.push_back(action);
}

Action *ActionList::Pop(int index)
{
    Action *output = actions_[index];
    actions_[index] = nullptr;
    return output;
}

Action *ActionList::Get(int index) const
{
    return actions_[index];
}

int ActionList::GetSize() const
{
    return actions_.size();
}

Game::~Game() {}
