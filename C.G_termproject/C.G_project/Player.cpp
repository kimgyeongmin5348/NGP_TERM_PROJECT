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
    glm::vec3 pos = GetPosition();
    glm::vec3 rotate = GetRotation();
    switch (state)
    {
    case LEFT:
        if (pos.x < 4)
            pos.x += 0.05f;
        if (rotate.z > -15)
            rotate.z -= 1.0f;
        break;
    case RIGHT:
        if (pos.x > -4)
            pos.x -= 0.05f;
        if (rotate.z < 15)
            rotate.z += 1.0f;
        break;
    case UP:
        if (pos.y < 4)
            pos.y += 0.03f;
        if (rotate.x < 15)
            rotate.x += 1.0f;
        break;
    case DOWN:
        if (pos.y > 0)
            pos.y -= 0.03f;
        if (rotate.x > -15)
            rotate.x -= 1.0f;
        break;
    default:
        if (rotate.x < 0.f) rotate.x += 1.0f;
        if (rotate.x > 0.f) rotate.x -= 1.0f;
        if (rotate.z > 0.f) rotate.z -= 1.0f;
        if (rotate.z < 0.f) rotate.z += 1.0f;
    }
    SetPosition(pos);
    SetRotation(rotate);
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

void Player::SetPosition(const glm::vec3& pos)
{ 
    position = pos;

    wingConnect->SetPosition(glm::vec3(pos.x, pos.y + 1.0f, pos.z));
    wingLeft->SetPosition(glm::vec3(pos.x, pos.y + 1.1f, pos.z));
    wingRight->SetPosition(glm::vec3(pos.x, pos.y + 1.1f, pos.z));
    body->SetPosition(glm::vec3(pos.x, pos.y + 0.7f, pos.z - 0.2f));
    bodyFront->SetPosition(glm::vec3(pos.x, pos.y + 0.65f, pos.z + 0.3f));
    bodyBack->SetPosition(glm::vec3(pos.x, pos.y + 0.7f, pos.z - 1.0f));
    tailFront->SetPosition(glm::vec3(pos.x, pos.y + 0.7f, pos.z - 1.7f));
    tailWing->SetPosition(glm::vec3(pos.x, pos.y + 0.8f, pos.z - 1.6f));
    bodyFloorLeft->SetPosition(glm::vec3(pos.x + 0.3f, pos.y + 0.2f, pos.z - 0.3f));
    bodyFloorRight->SetPosition(glm::vec3(pos.x - 0.3f, pos.y + 0.2f, pos.z - 0.3f));
}

void Player::SetRotation(const glm::vec3& rot)
{
    rotation = rot;

    wingConnect->SetRotation(glm::vec3(rot.x, rot.y, rot.z));
    wingLeft->SetRotation(glm::vec3(rot.x, wingLeft->GetRotation().y, rot.z));
    wingRight->SetRotation(glm::vec3(rot.x, wingRight->GetRotation().y, rot.z));
    body->SetRotation(glm::vec3(rot.x, 0, rot.z));
    bodyFront->SetRotation(glm::vec3(rot.x, 0, rot.z));
    bodyBack->SetRotation(glm::vec3(rot.x, 0, rot.z));
    tailFront->SetRotation(glm::vec3(rot.x, 0, rot.z));
    tailWing->SetRotation(glm::vec3(rot.x, 0, rot.z));
    bodyFloorLeft->SetRotation(glm::vec3(rot.x, 0, rot.z));
    bodyFloorRight->SetRotation(glm::vec3(rot.x, 0, rot.z));

}
