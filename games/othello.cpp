#include "othello.h"

Othello::Othello() : board_(), whos_turn_(Player::PLAYER1), last_skipped_(false), gameover_(false)
{
    board_[3 * 8 + 3] = PieceType::PLAYER1;
    board_[4 * 8 + 4] = PieceType::PLAYER1;
    board_[3 * 8 + 4] = PieceType::PLAYER2;
    board_[4 * 8 + 3] = PieceType::PLAYER2;
}
Othello::Othello(const Othello *state) : whos_turn_(state->whos_turn_), last_skipped_(state->last_skipped_), gameover_(state->gameover_)
{
    for (int i = 0; i < 64; i++)
    {
        board_[i] = state->board_[i];
    }
}
Othello::~Othello() {}

Othello *Othello::Clone() const
{
    return new Othello(this);
}

ActionList *Othello::GetLegalMoves() const
{
    ActionList *output = new ActionList();
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int position = i * 8 + j;
            if (board_[position] != PieceType::EMPTY)
                continue;
            if (canFlip(i, j, 0, -1) || canFlip(i, j, 0, 1) ||
                canFlip(i, j, -1, 0) || canFlip(i, j, 1, 0) ||
                canFlip(i, j, 1, 1) || canFlip(i, j, -1, -1) ||
                canFlip(i, j, -1, 1) || canFlip(i, j, 1, -1))
            {
                output->Add(new OthelloAction(position));
            }
        }
    }
    if (output->GetSize() == 0)
    {
        output->Add(new OthelloAction(-1));
    }

    // for (int i = 0; i < output->GetSize(); i++)
    // {
    //     std::cout << output->Get(i)->encoding() << " ";
    // }
    // std::cout << std::endl;
    return output;
}

void Othello::DoAction(const Action *action_abs)
{
    OthelloAction *action = (OthelloAction *)action_abs;
    int position = action->position();
    if (position == -1)
    {
        if (last_skipped_)
            gameover_ = true;

        last_skipped_ = true;
        switch_turn();
        return;
    }
    int i = position / 8;
    int j = position % 8;

    PieceType cur_piece = whos_turn_ == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    doFlip(i, j, 0, -1);
    doFlip(i, j, 0, 1);
    doFlip(i, j, -1, 0);
    doFlip(i, j, 1, 0);
    doFlip(i, j, -1, 1);
    doFlip(i, j, -1, -1);
    doFlip(i, j, 1, 1);
    doFlip(i, j, 1, -1);
    board_[i * 8 + j] = cur_piece;
    last_skipped_ = false;
    switch_turn();
}

Player Othello::GetPlayerThisTurn() const
{
    return whos_turn_;
}

ResultType Othello::GetResult() const
{
    if (!gameover_)
        return ResultType::NOT_FINISH_YET;
    int p1 = 0;
    int p2 = 0;
    for (int i = 0; i < 64; i++)
    {
        if (board_[i] == PieceType::PLAYER1)
            p1++;
        else if (board_[i] == PieceType::PLAYER2)
            p2++;
    }
    if (p1 > p2)
        return ResultType::PLAYER1_WIN;
    else if (p1 < p2)
        return ResultType::PLAYER2_WIN;
    else
        return ResultType::DRAW;
}

bool Othello::IsGameOver() const
{
    return gameover_;
}

void Othello::PrintState() const
{

    std::cout << "Current state" << std::endl;
    for (int i = 0; i < 8; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            char c;
            if (board_[i * 8 + j] == PieceType::EMPTY)
                c = ' ';
            else if (board_[i * 8 + j] == PieceType::PLAYER1)
                c = 'O';
            else if (board_[i * 8 + j] == PieceType::PLAYER2)
                c = 'X';
            else
                c = '?';
            std::cout << "|" << c;
        }
        std::cout << "|" << std::endl;
    }
    for (int i = 0; i < 8; i++)
    {
        std::cout << "--";
    }
    std::cout << "-" << std::endl;
    std::cout << std::endl;
}

void Othello::switch_turn()
{
    whos_turn_ = whos_turn_ == Player::PLAYER1 ? Player::PLAYER2 : Player::PLAYER1;
}

bool Othello::canFlip(int8_t i, int8_t j, int8_t di, int8_t dj) const
{
    int8_t temp_i = i + 2 * di;
    int8_t temp_j = j + 2 * dj;
    if (temp_i < 0 || temp_i >= 8 || temp_j < 0 || temp_j >= 8)
        return false;

    PieceType cur_piece = whos_turn_ == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    int flip = 0;
    i += di;
    j += dj;
    while (0 <= i && i < 8 && 0 <= j && j < 8)
    {
        if (board_[i * 8 + j] == PieceType::EMPTY)
            return false;
        else if (board_[i * 8 + j] == cur_piece)
            return flip > 0;
        else
            flip++;
        i += di;
        j += dj;
    }
    return false;
}

void Othello::doFlip(int8_t i, int8_t j, int8_t di, int8_t dj)
{
    int8_t temp_i = i + 2 * di;
    int8_t temp_j = j + 2 * dj;
    if (temp_i < 0 || temp_i >= 8 || temp_j < 0 || temp_j >= 8)
        return;

    PieceType cur_piece = whos_turn_ == Player::PLAYER1 ? PieceType::PLAYER1 : PieceType::PLAYER2;
    int flip = 0;
    i += di;
    j += dj;
    while (0 <= i && i < 8 && 0 <= j && j < 8)
    {
        if (board_[i * 8 + j] == PieceType::EMPTY)
            return;
        else if (board_[i * 8 + j] == cur_piece)
        {
            bool legal = flip > 0;
            while (flip--)
            {
                i -= di;
                j -= dj;
                board_[i * 8 + j] = cur_piece;
            }
            return;
        }
        else
            flip++;
        i += di;
        j += dj;
    }
    return;
}

bool Othello::IsSame(const Game *input_state) const
{
    Othello *state = (Othello *)input_state;
    if (this->whos_turn_ != state->whos_turn_)
        return false;
    if (this->last_skipped_ != state->last_skipped_)
        return false;
    if (this->gameover_ != state->gameover_)
        return false;
    for (int i = 0; i < 64; i++)
    {
        if (this->board_[i] != state->board_[i])
            return false;
    }
    return true;
}
