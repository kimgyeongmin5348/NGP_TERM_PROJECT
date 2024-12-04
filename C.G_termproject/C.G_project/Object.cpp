#include "stdafx.h"
#include "Object.h"
#include "PacketDefine.h"

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

    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);

    glBindVertexArray(0);
}

Object::~Object()
{
}

void Object::Render(GLuint program, int type) 
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::scale(transform, scale);

    GLuint transformLoc = glGetUniformLocation(program, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

    GLuint colorLoc = glGetUniformLocation(program, "objectColor");
    glUniform3fv(colorLoc, 1, glm::value_ptr(color));

    glBindVertexArray(VAO);
    if (type == 1) gluSphere(qobj, 1.0, 20, 30);
    else if (type == 2) gluCylinder(qobj, 0.3f, 1.1f, 0.5, 100, 1);
    else if (type == 3) gluCylinder(qobj, 0.3f, 0.3f, 1.5, 100, 1);
    else glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void Object::Update(float deltaTime)
{
}

/////////////////////////////////////////////////////////////////////////

RotatingObject::RotatingObject()
{
}

RotatingObject::~RotatingObject()
{
}

void RotatingObject::Update(float deltaTime)
{
    rotation.y += 15.0f;
}

/////////////////////////////////////////////////////////////////////////

Bullet::Bullet()
{
    SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
    SetColor(glm::vec3(0.3f, 0.3f, 0.4f));
}

Bullet::~Bullet()
{
}

void Bullet::Update(float deltaTime)
{
    if (!active) return;

    position.z += 0.5f;

    // 범위를 벗어나면 비활성화
    if (position.z > 50.0f) {
        active = false;
        this->SetPosition(glm::vec3(1000, 1000, 1000));
    }
    // [To Server] Send PacketPlayerMove
    //char type = PACKET_BULLET_MOVE;
    //int retval = send(sock, &type, sizeof(char), 0);
    //if (retval == SOCKET_ERROR)
    //    err_display("Send() - PACKET_BULLET_MOVE.type");

    //PacketBulletMove pbm;
    //pbm.size = sizeof(PacketBulletMove);
    //pbm.type = PACKET_BULLET_MOVE;
    //pbm.pos = position;

    //retval = send(sock, (char*)&pbm, sizeof(PacketBulletMove), 0);
    //if (retval == SOCKET_ERROR)
    //    err_display("send() - PACKET_BULLET_MOVE");

}

/////////////////////////////////////////////////////////////////////////

Building::Building()
{
    SetPosition(glm::vec3(position.x, 0.f, position.z));
    SetScale(glm::vec3(2.0f, scale.y, 4.0f));
    SetColor(glm::vec3(1.0f, 0.0f, 0.5f));
}

Building::~Building()
{
}

void Building::Setting(int j)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> random_building_x_pos(-20, 20);
    std::uniform_real_distribution<float> random_building_hight(1, 25);

    position.x = random_building_x_pos(gen);
    position.y = 0;
    position.z = 20.f * (j + 1);
    scale.y = random_building_hight(gen);
}

void Building::Update(float deltaTime)
{
    //position.z -= 0.1f;

    if(position.z < 0.f) {
        Setting(9);
    }
}

/////////////////////////////////////////////////////////////////////////

Ground::Ground()
{
    SetScale(glm::vec3(1000.0f, 0.f, 1000.0f));
    SetColor(glm::vec3(0.1f, 0.1f, 0.1f));
}

Ground::~Ground()
{
}