#pragma once
#include "Object.h"

#define LEFT  0
#define RIGHT 1
#define UP    2
#define DOWN  3

class Scene;

class Player : public Object
{
public:
	Player(Scene* scene);
	~Player();

	void SetPosition(const glm::vec3& pos) override;
	void SetRotation(const glm::vec3& rot) override;


	virtual void Update(float deltaTime) override;
	virtual void Render(GLuint program);

	// ��Ʈ��ũ ���� ��� ����
	bool isReady;                    // �κ񿡼��� �غ� ����
	char playerid[32];              // �÷��̾� ID (MAX_ID_SIZE = 32)
	int state;                      // �÷��̾� ���� (���� ��)


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

	Scene* scene;
};

