#include "stdafx.h"
#include "Player.h"

Player::Player()
{
    // 몸통
    body = new Object();
    body->SetPosition(glm::vec3(0.f, 0.7f, -0.2f));
    body->SetScale(glm::vec3(0.2f, 0.5f, 0.2f));
    body->SetColor(glm::vec3(0.5f, 0.5f, 0.5f));
    SetParent(body);

    // 왼쪽 날개
    wingLeft = new Object();
    wingLeft->SetPosition(glm::vec3(-0.5f, 0.0f, 0.0f));
    wingLeft->SetScale(glm::vec3(0.5f, 0.1f, 0.1f));
    wingLeft->SetColor(glm::vec3(0.2f, 0.2f, 0.8f));
    AddChild(wingLeft);

    // 오른쪽 날개
    wingRight = new Object();
    wingRight->SetPosition(glm::vec3(0.5f, 0.0f, 0.0f));
    wingRight->SetScale(glm::vec3(0.5f, 0.1f, 0.1f));
    wingRight->SetColor(glm::vec3(0.2f, 0.2f, 0.8f));
    AddChild(wingRight);

    // 꼬리
    tail = new Object();
    tail->SetPosition(glm::vec3(0.0f, -0.5f, -0.3f));
    tail->SetScale(glm::vec3(0.1f, 0.3f, 0.1f));
    tail->SetColor(glm::vec3(0.2f, 0.8f, 0.2f));
    AddChild(tail);
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
    rotation.y += 45.0f * deltaTime;
}

void Player::Render(GLuint program, const glm::mat4& parentTransform)
{
    Object::Render(program, parentTransform);
}
