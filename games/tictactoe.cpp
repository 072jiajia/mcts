#include "tictactoe.h"

TicTacToe::TicTacToe() : board(), whos_turn(Player::PLAYER1) {}

TicTacToe::~TicTacToe() {}

TicTacToe::TicTacToe(TicTacToe *src)
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i][j] = src->board[i][j];
        }
    }
    whos_turn = src->whos_turn;
}

std::vector<TicTacToe::Action> TicTacToe::GetLegalMoves()
{
    std::vector<Action> output;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == PieceType::EMPTY)
                output.emplace_back(i, j);
        }
    }
    return output;
}

void TicTacToe::DoAction(const Action action)
{
    if (board[action.x][action.y] != PieceType::EMPTY)
    {
        throw std::invalid_argument("Your action is invalid");
    }
    board[action.x][action.y] = whos_turn == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    switch_turn();
}

TicTacToe *TicTacToe::Clone()
{
    return new TicTacToe(this);
}
void TicTacToe::switch_turn()
{
    whos_turn = whos_turn == Player::PLAYER1 ? Player::PLAYER2 : Player::PLAYER1;
}

void TicTacToe::PrintState()
{

    std::cout << "Current state" << std::endl;
    for (int i = 0; i < 3; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
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
        std::cout << "|" << std::endl;
    }
    for (int i = 0; i < 3; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;
    std::cout << std::endl;
}

bool TicTacToe::IsGameOver()
{
    ResultType result = GetResult();
    return result != ResultType::NOT_FINISH_YET;
}

TicTacToe::ResultType TicTacToe::GetResult()
{
    for (int i = 0; i < 3; i++)
    {
        if (board[i][0] == PieceType::EMPTY)
            continue;
        bool all_same = true;
        for (int j = 1; j < 3; j++)
        {
            if (board[i][j - 1] != board[i][j])
            {
                all_same = false;
                break;
            }
        }
        if (all_same)
        {
            return board[i][0] == PieceType::PLAYER1 ? ResultType::PLAYER1_WIN : ResultType::PLAYER2_WIN;
        }
    }
    for (int i = 0; i < 3; i++)
    {
        if (board[0][i] == PieceType::EMPTY)
            continue;
        bool all_same = true;
        for (int j = 1; j < 3; j++)
        {
            if (board[j - 1][i] != board[j][i])
            {
                all_same = false;
                break;
            }
        }
        if (all_same)
        {
            return board[0][i] == PieceType::PLAYER1 ? ResultType::PLAYER1_WIN : ResultType::PLAYER2_WIN;
        }
    }

    if (board[0][0] != PieceType::EMPTY)
    {
        bool all_same = true;
        for (int j = 1; j < 3; j++)
        {
            if (board[j - 1][j - 1] != board[j][j])
            {
                all_same = false;
                break;
            }
        }
        if (all_same)
        {
            return board[0][0] == PieceType::PLAYER1 ? ResultType::PLAYER1_WIN : ResultType::PLAYER2_WIN;
        }
    }

    if (board[0][2] != PieceType::EMPTY)
    {
        bool all_same = true;
        for (int j = 1; j < 3; j++)
        {
            if (board[j - 1][3 - j] != board[j][2 - j])
            {
                all_same = false;
                break;
            }
        }
        if (all_same)
        {
            return board[0][2] == PieceType::PLAYER1 ? ResultType::PLAYER1_WIN : ResultType::PLAYER2_WIN;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (board[i][j] == PieceType::EMPTY)
                return ResultType::NOT_FINISH_YET;
        }
    }
    return ResultType::DRAW;
}

TicTacToe::Player TicTacToe::GetPlayerThisTurn() { return whos_turn; }
