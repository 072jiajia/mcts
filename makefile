simpledemo:
	g++ main.cpp src/utils.cpp games/tictactoe.cpp -o simpledemo

clean-simpledemo:
	rm simpledemo

demo-nogo:
	g++ demo-nogo.cpp src/utils.cpp games/nogo.cpp -o demo-nogo

clean-demo-nogo:
	rm demo-nogo


