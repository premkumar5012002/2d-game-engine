CC = g++
LAND_STD = -std=c++17
COMPILER_FLAGS = -Wall -Wfatal-errors
INCLUDE_PATH = -Ilibs
SRC_FILES = src/*.cpp src/Game/*.cpp src/Logger/*.cpp
LINKER_FLAGS = -llua5.3 -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
OBJ_NAME = 2d-game-engine

build:
	$(CC) $(COMPILER_FLAGS) $(LAND_STD) $(INCLUDE_PATH) $(SRC_FILES) $(LINKER_FLAGS) -o$(OBJ_NAME)

run:
	./$(OBJ_NAME)

clean:
	rm $(OBJ_NAME)