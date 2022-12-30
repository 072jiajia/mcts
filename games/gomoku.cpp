#include "gomoku.h"

Gomoku::Gomoku() : remaining_space_(225), board(), whos_turn(Player::PLAYER1), result_(ResultType::NOT_FINISH_YET) {}

Gomoku::~Gomoku() {}

Gomoku::Gomoku(const Gomoku *src)
    : remaining_space_(src->remaining_space_),
      whos_turn(src->whos_turn), result_(src->result_)
{
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            board[i][j] = src->board[i][j];
        }
    }
}

ActionList *Gomoku::GetLegalMoves() const
{
    ActionList *output = new ActionList();
    for (int i = 0; i < 15; i++)
    {
        for (int j = 0; j < 15; j++)
        {
            if (board[i][j] == PieceType::EMPTY)
                output->Add(new GomokuAction(i, j));
        }
    }
    return output;
}

void Gomoku::DoAction(const Action *action_abs)
{
    GomokuAction *action = (GomokuAction *)action_abs;
    int x = action->x();
    int y = action->y();
    if (board[x][y] != PieceType::EMPTY)
    {
        throw std::invalid_argument("Your action is invalid");
    }
    board[x][y] = whos_turn == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    remaining_space_--;

    if (LineConnected(x, y, 1, 1) ||
        LineConnected(x, y, 1, 0) ||
        LineConnected(x, y, 0, 1) ||
        LineConnected(x, y, 1, -1))
    {
        result_ = whos_turn == Player::PLAYER1 ? ResultType::PLAYER1_WIN : ResultType::PLAYER2_WIN;
    }
    else if (remaining_space_ <= 0)
    {
        result_ = ResultType::DRAW;
    }

    switch_turn();
}

Gomoku *Gomoku::Clone() const
{
    return new Gomoku(this);
}
void Gomoku::switch_turn()
{
    whos_turn = whos_turn == Player::PLAYER1 ? Player::PLAYER2 : Player::PLAYER1;
}

void Gomoku::PrintState() const
{
    std::cout << "Current state" << std::endl;

    std::cout << "   ";
    for (int i = 0; i < 15; i++)
    {
        std::cout << " " << char('A' + i);
    }
    std::cout << " " << std::endl;

    for (int i = 0; i < 15; i++)
    {
        if (i + 1 < 10)
            std::cout << " " << i + 1 << " ";
        else
            std::cout << i + 1 << " ";

        for (int j = 0; j < 15; j++)
        {
            char c;
            if (board[i][j] == PieceType::EMPTY)
                c = ' ';
            else if (board[i][j] == PieceType::PLAYER1)
                c = 'O';
            else if (board[i][j] == PieceType::PLAYER2)
                c = 'X';
            else
                c = '?';
            std::cout << "|" << c;
        }
        std::cout << "| " << i + 1 << std::endl;
    }

    std::cout << "   ";
    for (int i = 0; i < 15; i++)
    {
        std::cout << " " << char('A' + i);
    }
    std::cout << " " << std::endl;

    std::cout << std::endl;
}

bool Gomoku::IsGameOver() const
{
    ResultType result = GetResult();
    return result != ResultType::NOT_FINISH_YET;
}

ResultType Gomoku::GetResult() const
{
    return result_;
}

Player Gomoku::GetPlayerThisTurn() const { return whos_turn; }

bool Gomoku::LineConnected(int x, int y, int dx, int dy) const
{
    PieceType piece = board[x][y];
    int line_length = 1;
    int i = x + dx;
    int j = y + dy;
    while (0 <= i && i < 15 && 0 <= j && j < 15 && board[i][j] == piece)
    {
        line_length++;
        i += dx;
        j += dy;
    }
    i = x - dx;
    j = y - dy;
    while (0 <= i && i < 15 && 0 <= j && j < 15 && board[i][j] == piece)
    {
        line_length++;
        i -= dx;
        j -= dy;
    }
    return line_length >= 5;
}