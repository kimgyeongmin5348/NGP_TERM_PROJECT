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
    }  // �⺻ ������ �߰�

    Player(Scene* scene);  // ���� ������
    virtual ~Player();

    void SetPosition(const glm::vec3& pos) override;
    void SetRotation(const glm::vec3& rot) override;

    virtual void Update(float deltaTime) override;
    virtual void Render(GLuint program);

    // ��Ʈ��ũ ���� ��� ����
    bool isReady;                    // �κ񿡼��� �غ� ����
    char playerid[32];              // �÷��̾� ID (MAX_ID_SIZE = 32)
    int state;                      // �÷��̾� ���� (���� ��)

        
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
        // for ����ȸ��
        wingLeft->Update(deltaTime);
        wingRight->Update(deltaTime);
    };
};