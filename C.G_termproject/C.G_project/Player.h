#pragma once
#include "Object.h"

class Scene;

class Player : public Object
{
public:
	Player(Scene* scene);
	~Player();

	void SetPosition(const glm::vec3& pos) override;


	virtual void Update(float deltaTime) override;
	virtual void Render(GLuint program);

	// 네트워크 관련 멤버 변수
	bool isReady;                    // 로비에서의 준비 상태
	char playerid[32];              // 플레이어 ID (MAX_ID_SIZE = 32)
	int state;                      // 플레이어 상태 (기울기 등)


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

