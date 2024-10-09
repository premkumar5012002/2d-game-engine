build:
	mkdir build && g++ -Wall -std=c++17 -Ilibs src/*.cpp -llua5.3 -lSDL2 -lSDL2_image -lSDL2_ttf -llua5.3 -o build/2d-game-engine

run:
	build/2d-game-engine

clean:
	rm -r build