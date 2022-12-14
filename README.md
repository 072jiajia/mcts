# Monte Carlo Tree Search

This is a c++ implementation of Monte Carlo Tree Search algorithm.

Also, this is a project for [myself](https://github.com/072jiajia) to practice software development,
so I may update it occasionally.
Of course, I will add other games & add further techniques of mcts.

If you have any advices for this project (including implementation / solid principles / naming convention / optimization / others...),
please feel free to leave a message on the issue page or send me an [email](mailto:jijiawu.cs@gmail.com) to let me know!

## Quick Start

```
    # compile
    make simpledemo
    # Run the demo on tic-tac-toe game
    ./simpledemo
```

## How To Run It On Your Game?

Implement your game and finish the functions which will be used in Monte Carlo Tree Search

Here's an example

your_game.h

```code=cpp
#include "games/game_base.h"

class YourGame : public Game
{
public:
    YourGame();
    YourGame(YourGame *);
    ~YourGame();

    YourGame *Clone();
    ActionList* GetLegalMoves();
    void DoAction(const Action *);
    Player GetPlayerThisTurn();
    ResultType GetResult();
    bool IsGameOver();
    void PrintState();

    /*
    other members & functions of your implementations
    */
};
```

your_game.cpp

```code=cpp
#inlcude "your_game.h"

ActionList* YourGame::GetLegalMoves() { /* your implementation */ }
void YourGame::DoAction(const Action action) { /* your implementation */ }
YourGame *YourGame::Clone() { /* your implementation */ }
void YourGame::switch_turn() { /* your implementation */ }
void YourGame::PrintState() { /* your implementation */ }
bool YourGame::IsGameOver() { /* your implementation */ }
ResultType YourGame::GetResult() { /* your implementation */ }
Player YourGame::GetPlayerThisTurn() { /* your implementation */ }

```

(On-going Readme.md contents...)
