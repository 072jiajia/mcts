simpledemo:
	mkdir -p build
	g++ -O3 demo-tictactoe.cpp src/*.cpp games/*.cpp -o build/simpledemo

clean-simpledemo:
	rm build/simpledemo

demo-nogo:
	mkdir -p build
	g++ -O3 demo-nogo.cpp src/*.cpp games/*.cpp -o build/demo-nogo

clean-demo-nogo:
	rm build/demo-nogo

demo-rave:
	mkdir -p build
	g++ -O3 demo-rave.cpp src/*.cpp games/*.cpp -o build/demo-rave

clean-demo-rave:
	rm build/demo-rave


