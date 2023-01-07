source_cpp_files := mcts/*.cpp mcts/*/*.cpp mcts/*/*/*.cpp games/*.cpp

quickdemo:
	mkdir -p build
	g++ -O3 -pthread test/test-tictactoe.cpp $(source_cpp_files) -o build/quickdemo

test-tictactoe:
	mkdir -p build
	g++ -O3 -pthread test/test-tictactoe.cpp $(source_cpp_files) -o build/test-tictactoe

test-nogo:
	mkdir -p build
	g++ -O3 -pthread test/test-nogo.cpp $(source_cpp_files) -o build/test-nogo

test-rave:
	mkdir -p build
	g++ -O3 -pthread test/test-rave.cpp $(source_cpp_files) -o build/test-rave

test-parallel:
	mkdir -p build
	g++ -O3 -pthread test/test-parallel.cpp $(source_cpp_files) -o build/test-parallel

test-othello:
	mkdir -p build
	g++ -O3 -pthread test/test-othello.cpp $(source_cpp_files) -o build/test-othello

test-gomoku:
	mkdir -p build
	g++ -O3 -pthread test/test-gomoku.cpp $(source_cpp_files) -o build/test-gomoku

test-connect-four:
	mkdir -p build
	g++ -O3 -pthread test/test-connect-four.cpp $(source_cpp_files) -o build/test-connect-four

test-moving-root:
	mkdir -p build
	g++ -O3 -pthread test/test-moving-root.cpp $(source_cpp_files) -o build/test-moving-root

play-connect-four:
	mkdir -p build
	g++ -O3 -pthread play-connect-four.cpp $(source_cpp_files) -o build/play-connect-four

clean-play-connect-four:
	rm build/play-connect-four
