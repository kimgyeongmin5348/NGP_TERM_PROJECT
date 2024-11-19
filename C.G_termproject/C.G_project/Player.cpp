#include "stdafx.h"
#include "Player.h"
#include "Scene.h"

Player::Player(Scene* scene)
{
    // 날개 연결부
    wingConnect = new Object();
    wingConnect->SetPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    wingConnect->SetScale(glm::vec3(0.2f, 0.5f, 0.2f));
    wingConnect->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 왼쪽 날개
    wingLeft = new RotatingObject();
    Scene::GetInstance()->AddGameObject(wingLeft);
    wingLeft->SetPosition(glm::vec3(0.0f, 1.1f, 0.0f));
    wingLeft->SetScale(glm::vec3(4.5f, 0.2f, 0.2f));
    wingLeft->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 오른쪽 날개
    wingRight = new RotatingObject();
    Scene::GetInstance()->AddGameObject(wingRight);
    wingRight->SetPosition(glm::vec3(0.0f, 1.1f, 0.0f));
    wingRight->SetScale(glm::vec3(0.2f, 0.2f, 4.5f));
    wingRight->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 몸통
    body = new Object();
    body->SetPosition(glm::vec3(0.f, 0.7f, -0.2f));
    body->SetScale(glm::vec3(1.1f, 1.1f, 3.0f));
    body->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 몸통 앞
    bodyFront = new Object();
    bodyFront->SetPosition(glm::vec3(0.f, 0.65f, 0.3f));
    bodyFront->SetScale(glm::vec3(0.3f, 0.3f, 0.4f));
    bodyFront->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 몸통 뒤
    bodyBack = new Object();
    bodyBack->SetPosition(glm::vec3(0.f, 0.7f, -1.0f));
    bodyBack->SetScale(glm::vec3(0.2f, 0.2f, 1.0f));
    bodyBack->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 꼬리 앞
    tailFront = new Object();
    tailFront->SetPosition(glm::vec3(0.f, 0.7f, -1.7f));
    tailFront->SetScale(glm::vec3(0.2f, 0.2f, 1.0f));
    tailFront->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 꼬리 날개
    tailWing = new Object();
    tailWing->SetPosition(glm::vec3(0.f, 0.8f, -1.6f));
    tailWing->SetScale(glm::vec3(0.1f, 0.7f, 0.5f));
    tailWing->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 본체 바닥 왼쪽
    bodyFloorLeft = new Object();
    bodyFloorLeft->SetPosition(glm::vec3(0.3f, 0.2f, -0.3f));
    bodyFloorLeft->SetScale(glm::vec3(0.2f, 0.2f, 2.5f));
    bodyFloorLeft->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));

    // 본체 바닥 오른쪽
    bodyFloorRight = new Object();
    bodyFloorRight->SetPosition(glm::vec3(-0.3f, 0.2f, -0.3f));
    bodyFloorRight->SetScale(glm::vec3(0.2f, 0.2f, 2.5f));
    bodyFloorRight->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
    rotation.y += 45.0f * deltaTime;
}

void Player::Render(GLuint program)
{
    body->Render(program, 0);
    wingLeft->Render(program, 0);
    wingRight->Render(program, 0);
    wingConnect->Render(program, 0);
    bodyFront->Render(program, 1);
    bodyBack->Render(program, 2);
    tailFront->Render(program, 3);
    tailWing->Render(program, 0);
    bodyFloorLeft->Render(program, 0);
    bodyFloorRight->Render(program, 0);
}
