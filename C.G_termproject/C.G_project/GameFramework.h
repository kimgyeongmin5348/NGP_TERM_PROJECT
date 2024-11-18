#pragma once
#include "stdafx.h"
#include "Scene.h"

class GameFramework {
public:
    void Initialize(int argc, char** argv);
    void Run();
    void Shutdown();

    static void TimerCallback(int value);

private:
    Scene* scene;
};
