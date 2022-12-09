simpledemo:
	g++ -D TICTACTOE main.cpp src/utils.cpp games/tictactoe.cpp -o simpledemo

clean-simpledemo:
	rm simpledemo

demo-nogo:
	g++ -D NOGO main.cpp src/utils.cpp games/nogo.cpp -o demo-nogo

clean-demo-nogo:
	rm demo-nogo


