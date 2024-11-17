#include "stdafx.h"
#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

/////////////////////////////////////////////////////////////////////////

Bullet::Bullet()
{
    glm::mat4 Bullet = glm::mat4(1.0f);
    Bullet = glm::translate(Bullet, glm::vec3(x_trans_aoc, y_trans_aoc, z_trans_aoc));
    Bullet = glm::rotate(Bullet, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));
    Bullet = glm::rotate(Bullet, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));
    Bullet = glm::scale(Bullet, glm::vec3(0.3f, 0.3f, 0.4f));

    //unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    //glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Bullet));
    //int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    //unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    //
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    //glUniform1f(isCheck, false);
    //glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
    //gluSphere(qobj, 0.2, 20, 30);
    // 한글 확인
}

Bullet::~Bullet()
{
}

/////////////////////////////////////////////////////////////////////////

Building::Building()
{
}

Building::~Building()
{
}
