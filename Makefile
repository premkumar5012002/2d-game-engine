build:
	mkdir build && g++ -Wall -std=c++17 src/Main.cpp -o build/2d-game-engine

run:
	build/2d-game-engine

clean:
	rm -r build