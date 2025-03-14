#include "./Game/Game.hpp"

int main(int argc, char* argv[]) {
    Game game;

    game.Initialize();
    game.Run();
    game.Destroy();

    return EXIT_SUCCESS;
}
