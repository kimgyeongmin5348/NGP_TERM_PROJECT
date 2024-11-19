// main.cpp
#include "GameFramework.h"
//#include "ServerToClient.h"

int main(int argc, char** argv) 
{
    GameFramework game;
    game.Initialize(argc, argv);
    while (true)
    {
        game.Run();
    }
    game.Shutdown();
}
