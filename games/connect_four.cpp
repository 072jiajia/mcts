#include "connect_four.h"

ConnectFour::ConnectFour()
    : board_(), line_height_(),
      whos_turn_(Player::PLAYER1),
      result_(ResultType::NOT_FINISH_YET),
      remaining_space_(42) {}

ConnectFour::~ConnectFour() {}

ConnectFour::ConnectFour(const ConnectFour *src)
    : whos_turn_(src->whos_turn_),
      result_(src->result_),
      remaining_space_(src->remaining_space_)
{
    for (int i = 0; i < 7; i++)
    {
        line_height_[i] = src->line_height_[i];
        for (int j = 0; j < 6; j++)
        {
            board_[i][j] = src->board_[i][j];
        }
    }
}

ActionList *ConnectFour::GetLegalMoves() const
{
    ActionList *output = new ActionList();
    for (int i = 0; i < 7; i++)
    {
        if (line_height_[i] < 6)
        {
            output->Add(new ConnectFourAction(i, line_height_[i]));
        }
    }
    return output;
}

void ConnectFour::DoAction(const Action *action_abs)
{
    ConnectFourAction *action = (ConnectFourAction *)action_abs;
    int x = action->x();
    int y = action->y();
    if (y == -1)
    {
        y = line_height_[x];
        if (y >= 6)
        {
            throw std::invalid_argument("Your action is invalid");
        }
    }
    else if (line_height_[x] != y)
    {
        throw std::invalid_argument("Your action is invalid");
    }
    ++line_height_[x];
    board_[x][y] = whos_turn_ == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    --remaining_space_;

    if (LineConnected(x, y, 1, 1) >= 4 ||
        LineConnected(x, y, 1, 0) >= 4 ||
        LineConnected(x, y, 0, 1) >= 4 ||
        LineConnected(x, y, 1, -1) >= 4)
    {
        result_ = whos_turn_ == Player::PLAYER1 ? ResultType::PLAYER1_WIN : ResultType::PLAYER2_WIN;
    }
    else if (remaining_space_ <= 0)
    {
        result_ = ResultType::DRAW;
    }

    switch_turn();
}

ConnectFour *ConnectFour::Clone() const
{
    return new ConnectFour(this);
}
void ConnectFour::switch_turn()
{
    whos_turn_ = whos_turn_ == Player::PLAYER1 ? Player::PLAYER2 : Player::PLAYER1;
}

void ConnectFour::PrintState() const
{

    std::cout << "Current state" << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < 7; i++)
    {
        std::cout << " " << i + 1;
    }
    std::cout << " " << std::endl;

    for (int j = 6 - 1; j >= 0; j--)
    {
        for (int i = 0; i < 7; i++)
        {
            char c;
            if (board_[i][j] == PieceType::EMPTY)
                c = ' ';
            else if (board_[i][j] == PieceType::PLAYER1)
                c = 'O';
            else if (board_[i][j] == PieceType::PLAYER2)
                c = 'X';
            else
                c = '?';
            std::cout << "|" << c;
        }
        std::cout << "|" << std::endl;
    }
    for (int i = 0; i < 7; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;
    std::cout << std::endl;
}

bool ConnectFour::IsGameOver() const
{
    ResultType result = GetResult();
    return result != ResultType::NOT_FINISH_YET;
}

ResultType ConnectFour::GetResult() const
{
    return result_;
}

int ConnectFour::LineConnected(int x, int y, int dx, int dy) const
{
    PieceType piece = board_[x][y];
    int line_length = 1;
    int i = x + dx;
    int j = y + dy;
    while (0 <= i && i < 7 && 0 <= j && j < 6 && board_[i][j] == piece)
    {
        line_length++;
        i += dx;
        j += dy;
    }
    i = x - dx;
    j = y - dy;
    while (0 <= i && i < 7 && 0 <= j && j < 6 && board_[i][j] == piece)
    {
        line_length++;
        i -= dx;
        j -= dy;
    }
    return line_length;
}

Player ConnectFour::GetPlayerThisTurn() const { return whos_turn_; }
