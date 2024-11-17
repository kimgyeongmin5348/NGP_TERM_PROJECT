#pragma once

#include "Player.h"

class Scene
{
public:
	Scene(Player *pPlayer);
	~Scene();

	void BuildObject();

protected:
	Player* player;
	Ground* ground;
	Building* build;
	//Building* temp_build[1000];
};

