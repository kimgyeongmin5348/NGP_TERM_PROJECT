// main.cpp
#include "GameFramework.h"

int main(int argc, char** argv) {
    GameFramework game;
    game.Initialize(argc, argv);
    while (true)
    {
        game.Run();
    }
    game.Shutdown();
}
