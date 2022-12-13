simpledemo:
	g++ demo-tictactoe.cpp src/*.cpp games/*.cpp -o simpledemo

clean-simpledemo:
	rm simpledemo

demo-nogo:
	g++ demo-nogo.cpp src/*.cpp games/*.cpp -o demo-nogo

clean-demo-nogo:
	rm demo-nogo


