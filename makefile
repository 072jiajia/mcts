simpledemo:
	mkdir -p build
	g++ -O3 -pthread demo-tictactoe.cpp mcts/*.cpp mcts/strategies/*.cpp games/*.cpp -o build/simpledemo

clean-simpledemo:
	rm build/simpledemo

demo-nogo:
	mkdir -p build
	g++ -O3 -pthread demo-nogo.cpp mcts/*.cpp mcts/strategies/*.cpp games/*.cpp -o build/demo-nogo

clean-demo-nogo:
	rm build/demo-nogo

demo-rave:
	mkdir -p build
	g++ -O3 -pthread demo-rave.cpp mcts/*.cpp mcts/strategies/*.cpp games/*.cpp -o build/demo-rave

clean-demo-rave:
	rm build/demo-rave

demo-parallel:
	mkdir -p build
	g++ -O3 -pthread demo-parallel.cpp mcts/*.cpp mcts/strategies/*.cpp games/*.cpp -o build/demo-parallel

clean-demo-parallel:
	rm build/demo-parallel

demo-othello:
	mkdir -p build
	g++ -O3 -pthread demo-othello.cpp mcts/*.cpp mcts/strategies/*.cpp games/*.cpp -o build/demo-othello

clean-demo-othello:
	rm build/demo-othello
