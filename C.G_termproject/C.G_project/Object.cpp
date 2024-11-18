#include "stdafx.h"
#include "Object.h"

Object::Object()
{
    // VAO 생성 및 바인딩
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Object::~Object()
{
    for (auto child : children) {
        delete child;
    }
}

void Object::SetParent(Object* parent)
{
    parent = parent;
}

void Object::AddChild(Object* child)
{
    child->SetParent(this);
    children.push_back(child);
}

void Object::Render(GLuint program, const glm::mat4& parentTransform) 
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);

    transform = parentTransform * transform;

    GLuint transformLoc = glGetUniformLocation(program, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);

    GLuint colorLoc = glGetUniformLocation(program, "objectColor");
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    // glBindVertexArray(0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);


    for (auto child : children) {
        child->Render(program, transform);
    }
}

void Object::Update(float deltaTime)
{
    for (auto child : children) {
        child->Update(deltaTime);
    }
}

/////////////////////////////////////////////////////////////////////////

Bullet::Bullet()
{
    SetScale(glm::vec3(0.3f, 0.3f, 0.4f));
    SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
}

Bullet::~Bullet()
{
}

void Bullet::Update(float deltaTime)
{
    if (!active) return;

    position.z += deltaTime;

    // 범위를 벗어나면 비활성화
    if (position.z > 50.0f || position.z < -50.0f) {
        active = false;
    }
}

/////////////////////////////////////////////////////////////////////////

Building::Building()
{
    SetPosition(glm::vec3(position.x, 0.f, position.z));
    SetScale(glm::vec3(2.0f, 12.5f, 4.0f));
    SetColor(glm::vec3(1.0f, 0.0f, 0.5f));
}

Building::~Building()
{
}

void Building::Setting()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> random_building_x_pos(-20, 20);
    std::uniform_real_distribution<float> random_building_hight(1, 25);

    position.x = random_building_x_pos(gen);
    position.y = 0;
    position.z = 40.f;
    scale.y = random_building_hight(gen);
}

/////////////////////////////////////////////////////////////////////////

Ground::Ground()
{
    SetScale(glm::vec3(1000.0f, 0.f, 1000.0f));
    SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
}
