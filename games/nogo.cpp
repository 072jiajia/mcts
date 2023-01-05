#include "nogo.h"

NoGo::NoGo(int size_x, int size_y)
    : size_x_(size_x),
      size_y_(size_y),
      whos_turn_(Player::PLAYER1)
{
    flat_board_ = new PieceType[size_x * size_y]();
}

NoGo::NoGo(const NoGo *src)
    : size_x_(src->size_x_),
      size_y_(src->size_y_),
      whos_turn_(src->whos_turn_)
{
    flat_board_ = new PieceType[size_x_ * size_y_];
    for (int i = size_x_ * size_y_ - 1; i >= 0; i--)
    {
        flat_board_[i] = src->flat_board_[i];
    }
}

NoGo::~NoGo() { delete flat_board_; }
NoGo *NoGo::Clone() const { return new NoGo(this); }

Player NoGo::GetPlayerThisTurn() const { return whos_turn_; }

ActionList *NoGo::GetLegalMoves() const
{
    ActionList *output = new ActionList();
    for (int i = 0; i < size_x_; i++)
    {
        for (int j = 0; j < size_y_; j++)
        {
            Action *action = new NoGoAction(i, j);
            if (this->IsMovable(action))
            {
                output->Add(action);
            }
            else
            {
                delete action;
            }
        }
    }
    return output;
}

void NoGo::DoAction(const Action *action_abs)
{
    NoGoAction *action = (NoGoAction *)action_abs;
    flat_board_[action->x() * size_y_ + action->y()] = GetPlayersPiece(whos_turn_);
    switch_turn();
}

NoGo::PieceType NoGo::GetPlayersPiece(Player p)
{
    return p == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
}

ResultType NoGo::GetResult() const
{
    for (int i = 0; i < size_x_; i++)
    {
        for (int j = 0; j < size_y_; j++)
        {
            NoGoAction action(i, j);
            if (this->IsMovable(&action))
            {
                return ResultType::NOT_FINISH_YET;
            }
        }
    }
    return whos_turn_ == Player::PLAYER1 ? ResultType::PLAYER2_WIN : ResultType::PLAYER1_WIN;
}

bool NoGo::IsGameOver() const
{
    ResultType result = GetResult();
    return result != ResultType::NOT_FINISH_YET;
}

void NoGo::switch_turn()
{
    whos_turn_ = whos_turn_ == Player::PLAYER1 ? Player::PLAYER2 : Player::PLAYER1;
}

void NoGo::PrintState() const
{
    std::cout << "Current state" << std::endl;
    for (int i = 0; i < size_x_; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;

    for (int i = 0; i < size_y_; i++)
    {
        for (int j = 0; j < size_x_; j++)
        {
            char c;
            if (flat_board_[j * size_y_ + i] == PieceType::EMPTY)
                c = ' ';
            else if (flat_board_[j * size_y_ + i] == PieceType::PLAYER1)
                c = 'O';
            else if (flat_board_[j * size_y_ + i] == PieceType::PLAYER2)
                c = 'X';
            else
                c = '?';
            std::cout << "|" << c;
        }
        std::cout << "|" << std::endl;
    }
    for (int i = 0; i < size_x_; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;
    std::cout << std::endl;
}

bool NoGo::IsMovable(const Action *action_abs) const
{
    const NoGo &state = *this;
    NoGoAction *action = (NoGoAction *)action_abs;
    int x = action->x();
    int y = action->y();
    if (state.flat_board_[x * state.size_y_ + y] != PieceType::EMPTY)
        return false;
    NoGo test(&state);
    PieceType who = state.whos_turn_ == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    PieceType opp = state.whos_turn_ == Player::PLAYER1 ? PieceType::PLAYER2 : PieceType::PLAYER1;
    test.flat_board_[x * state.size_y_ + y] = who;
    if (!DoesHaveLiberty(test, x, y, who))
        return false;
    if (x - 1 >= 0 && !DoesHaveLiberty(test, x - 1, y, opp))
        return false;
    if (x + 1 < state.size_x_ && !DoesHaveLiberty(test, x + 1, y, opp))
        return false;
    if (y - 1 >= 0 && !DoesHaveLiberty(test, x, y - 1, opp))
        return false;
    if (y + 1 < state.size_y_ && !DoesHaveLiberty(test, x, y + 1, opp))
        return false;

    return true;
}

bool NoGo::DoesHaveLiberty(NoGo &state, int x, int y, PieceType who)
{
    if (state.flat_board_[state.size_y_ * x + y] != who)
        return true;

    NoGo test_state(&state);

    std::list<Position> check;
    for (check.emplace_back(x, y); check.size(); check.pop_front())
    {
        // std::cout << check.size() << std::endl;
        int x = check.front().x, y = check.front().y;
        test_state.flat_board_[x * test_state.size_y_ + y] = PieceType::UNKNOWN; // prevent recalculate

        if (x > 0)
        {
            PieceType near_l = test_state.flat_board_[(x - 1) * test_state.size_y_ + y]; // left
            if (near_l == PieceType::EMPTY)
                return true;
            else if (near_l == who)
                check.emplace_back(x - 1, y);
        }

        if (x < test_state.size_x_ - 1)
        {
            PieceType near_r = test_state.flat_board_[(x + 1) * test_state.size_y_ + y]; // right
            if (near_r == PieceType::EMPTY)
                return true;
            else if (near_r == who)
                check.emplace_back(x + 1, y);
        }

        if (y > 0)
        {
            PieceType near_d = test_state.flat_board_[x * test_state.size_y_ + y - 1]; // down
            if (near_d == PieceType::EMPTY)
                return true;
            else if (near_d == who)
                check.emplace_back(x, y - 1);
        }

        if (y < test_state.size_y_ - 1)
        {
            PieceType near_u = test_state.flat_board_[x * test_state.size_y_ + y + 1]; // up
            if (near_u == PieceType::EMPTY)
                return true;
            else if (near_u == who)
                check.emplace_back(x, y + 1);
        }
    }
    return false;
}

ActionList *NoGo::GetActionSpace(int size_x, int size_y)
{
    ActionList *output = new ActionList();
    for (int i = 0; i < size_x; i++)
    {
        for (int j = 0; j < size_y; j++)
        {
            output->Add(new NoGoAction(i, j));
        }
    }
    return output;
}