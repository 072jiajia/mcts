quickdemo:
	mkdir -p build
	g++ -O3 -pthread test/test-tictactoe.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/quickdemo

test-tictactoe:
	mkdir -p build
	g++ -O3 -pthread test/test-tictactoe.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-tictactoe

test-nogo:
	mkdir -p build
	g++ -O3 -pthread test/test-nogo.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-nogo

test-rave:
	mkdir -p build
	g++ -O3 -pthread test/test-rave.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-rave

test-parallel:
	mkdir -p build
	g++ -O3 -pthread test/test-parallel.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-parallel

test-othello:
	mkdir -p build
	g++ -O3 -pthread test/test-othello.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-othello

test-gomoku:
	mkdir -p build
	g++ -O3 -pthread test/test-gomoku.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-gomoku

test-connect-four:
	mkdir -p build
	g++ -O3 -pthread test/test-connect-four.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/test-connect-four

play-connect-four:
	mkdir -p build
	g++ -O3 -pthread play-connect-four.cpp mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp -o build/play-connect-four

clean-play-connect-four:
	rm build/play-connect-four
