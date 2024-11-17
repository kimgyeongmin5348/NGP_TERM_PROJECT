#pragma once

#include "Object.h"

class Player : public Object
{
public:
	Player();
	~Player();

public:
	Bullet* bullet;
};

