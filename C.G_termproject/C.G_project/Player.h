#pragma once
#include "Object.h"

class Player : public Object
{
public:
	Player();
	~Player();

	virtual void Update(float deltaTime) override;
	virtual void Render(GLuint program);

private:
	Object* body;
	Object* wingLeft;
	Object* wingRight;
	Object* wingConnect;
	Object* bodyFront;
	Object* bodyBack;
	Object* tailFront;
	Object* tailWing;
	Object* bodyFloorLeft;
	Object* bodyFloorRight;
};

