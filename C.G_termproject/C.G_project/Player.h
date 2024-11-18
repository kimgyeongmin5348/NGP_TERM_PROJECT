#pragma once
#include "Object.h"

class Player : public Object
{
public:
	Player();
	~Player();

	virtual void Update(float deltaTime) override;
	virtual void Render(GLuint program, const glm::mat4& parentTransform = glm::mat4(1.0f)) override;

private:
	Object* body;
	Object* wingLeft;
	Object* wingRight;
	Object* tail;
};

