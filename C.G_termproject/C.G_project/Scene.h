#pragma once

#include "Object.h"
#include "Player.h"

class Scene
{
public:
	Scene();
	~Scene();

	void BuildObject();

protected:
	Player* player;
	Bullet* bullet;
	Building* build[1000];
	Building* temp_build[1000];
};

