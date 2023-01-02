# Monte Carlo Tree Search

This is a c++ implementation of Monte Carlo Tree Search algorithm.

## Introduction

Monte Carlo tree search (MCTS) is an algorithm that is used to find the next best move in a game or a decision-making problem. It is particularly effective since it is able to balance the exploration of new game states with the exploitation of known good moves.
Some parallel implementation of MCTS can further enhance its performance.

## Quick Start

```
    # compile
    make quickdemo
    # Run the demo on tic-tac-toe game
    ./build/quickdemo
```

## Supported Algorithms

- MCTS
- [RAVE](https://doi.org/10.1016/j.artint.2011.03.007)
- [Root Parallel MCTS](https://dke.maastrichtuniversity.nl/m.winands/documents/multithreadedMCTS2.pdf)
- [Leaf Parallel MCTS](https://dke.maastrichtuniversity.nl/m.winands/documents/multithreadedMCTS2.pdf)
- [Tree Parallel MCTS](https://dke.maastrichtuniversity.nl/m.winands/documents/multithreadedMCTS2.pdf)

## Supported Board Games

- [Tic-tac-toe](https://en.wikipedia.org/wiki/Tic-tac-toe)
- [Othello (Reversi)](https://en.wikipedia.org/wiki/Reversi)
- [NoGo](https://webdocs.cs.ualberta.ca/~mmueller/nogo/rules.html)
- [Gomoku](https://en.wikipedia.org/wiki/Gomoku)
- [Connect Four](https://en.wikipedia.org/wiki/Connect_Four)
- [Other Games](#How-To-Run-It-On-Your-Game)

## Play Connect Four With MCTS Agent

```
    make play-connect-four
    ./build/play-connect-four
```

## How To Run It On Your Game?

Implement your game and finish the functions which will be used in Monte Carlo Tree Search

Here's an example

```code=cpp
#include "mcts/game_base.h"

class YourGameAction : public Action
{
    /*
        Your implementations of your game's action
    */
};

class YourGame : public Game
{
public:
    YourGame();
    YourGame(YourGame *);
    ~YourGame();

    YourGame *Clone() const;
    ActionList* GetLegalMoves() const;
    void DoAction(const Action *);
    Player GetPlayerThisTurn() const;
    ResultType GetResult() const;
    bool IsGameOver() const;
    void PrintState() const;

    /*
        Other members & functions of your implementations
    */
};
```

## Other Contents

BTW, this is a project for [myself](https://github.com/072jiajia) to practice software development,
so I may update it occasionally.
Of course, I will add other games & add further techniques of mcts.

If you have any advices for this project (including implementation / solid principles / naming convention / optimization / others...),
please feel free to leave a message on the issue page or send me an [email](mailto:jijiawu.cs@gmail.com) to let me know!
