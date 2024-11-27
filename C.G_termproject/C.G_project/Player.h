#pragma once
#include "Object.h"

#define LEFT  1
#define RIGHT 2
#define UP    3
#define DOWN  4
#define ARE_YOU_READY 5

class Scene;

class Player : public Object
{
public:
    Player() : scene(nullptr), body(nullptr), wingLeft(nullptr), wingRight(nullptr),
        wingConnect(nullptr), bodyFront(nullptr), bodyBack(nullptr),
        tailFront(nullptr), tailWing(nullptr), bodyFloorLeft(nullptr),
        bodyFloorRight(nullptr), isReady(false), state(0) {
        memset(playerid, 0, sizeof(playerid));
    }  // 기본 생성자 추가

    Player(Scene* scene);  // 기존 생성자
    virtual ~Player();

    void SetPosition(const glm::vec3& pos) override;
    void SetRotation(const glm::vec3& rot) override;

    virtual void Update(float deltaTime) override;
    virtual void Render(GLuint program);

    // 네트워크 관련 멤버 변수
    bool isReady;                    // 로비에서의 준비 상태
    char playerid[32];              // 플레이어 ID (MAX_ID_SIZE = 32)
    int state;                      // 플레이어 상태 (기울기 등)

        
protected:
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

class OtherPlayer : public Player
{
public:
    OtherPlayer() {};
    ~OtherPlayer() {};

    OtherPlayer(Scene* scene) : Player(scene) {};

    void Update(float deltaTime) override 
    {
        // for 날개회전
        wingLeft->Update(deltaTime);
        wingRight->Update(deltaTime);
    };
};