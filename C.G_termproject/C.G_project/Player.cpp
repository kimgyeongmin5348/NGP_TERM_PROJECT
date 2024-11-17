#include "stdafx.h"
#include "Player.h"

Player::Player()
{
    // 날개 연결부
    glm::mat4 H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.5f, 0.2f));
    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 날개 1
    glm::mat4 H_Matrix1 = glm::mat4(1.0f);
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(0.0f, 1.1f, 0.0f));
    H_Matrix1 = glm::scale(H_Matrix1, glm::vec3(4.5f, 0.2f, 0.2f));

    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix1));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 날개 2
    glm::mat4 H_Matrix2 = glm::mat4(1.0f);
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(0.0f, 1.1f, 0.0f));
    H_Matrix2 = glm::scale(H_Matrix2, glm::vec3(0.2f, 0.2f, 4.5f));

    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix2));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 몸통 중간
    glm::mat4 H_Matrix3 = glm::mat4(1.0f);
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix3 = glm::rotate(H_Matrix3, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix3 = glm::rotate(H_Matrix3, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(0.f, 0.7f, -0.2f));
    H_Matrix3 = glm::scale(H_Matrix3, glm::vec3(1.1f, 1.1f, 3.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix3));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 몸통 앞
    glm::mat4 H_Matrix4 = glm::mat4(1.0f);
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(x_trans_aoc, 0.f, 0.f));
    H_Matrix4 = glm::rotate(H_Matrix4, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));
    H_Matrix4 = glm::rotate(H_Matrix4, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(0.f, 0.65f, 0.3f));
    H_Matrix4 = glm::scale(H_Matrix4, glm::vec3(0.3f, 0.3f, 0.4f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix4));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    gluSphere(qobj, 1.0, 20, 30);

    // 몸통 뒤
    glm::mat4 H_Matrix5 = glm::mat4(1.0f);
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix5 = glm::rotate(H_Matrix5, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix5 = glm::rotate(H_Matrix5, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(0.f, 0.7f, -1.0f));
    H_Matrix5 = glm::scale(H_Matrix5, glm::vec3(0.2f, 0.2f, 1.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix5));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    gluCylinder(qobj, 0.3f, 1.1f, 0.5, 100, 1);

    // 몸통 뒤(꼬리앞)
    glm::mat4 H_Matrix6 = glm::mat4(1.0f);
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix6 = glm::rotate(H_Matrix6, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix6 = glm::rotate(H_Matrix6, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(0.f, 0.7f, -1.7f));
    H_Matrix6 = glm::scale(H_Matrix6, glm::vec3(0.2f, 0.2f, 1.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix6));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    gluCylinder(qobj, 0.3f, 0.3f, 1.5, 100, 1);

    // 몸통 뒤(꼬리 날개)
    glm::mat4 H_Matrix7 = glm::mat4(1.0f);
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));  // all
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix7 = glm::rotate(H_Matrix7, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix7 = glm::rotate(H_Matrix7, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(0.f, 0.8f, -1.6f));
    H_Matrix7 = glm::scale(H_Matrix7, glm::vec3(0.1f, 0.7f, 0.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix7));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //본체 바닥(왼쪽)
    H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));
    H_Matrix = glm::translate(H_Matrix, glm::vec3(x_trans_aoc, 0.f, 0.f));
    H_Matrix = glm::rotate(H_Matrix, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.3f, 0.2f, -0.3f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.2f, 2.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //본체 바닥2
    H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, y_trans_aoc, z_trans_aoc));
    H_Matrix = glm::translate(H_Matrix, glm::vec3(x_trans_aoc, 0.f, 0.f));
    H_Matrix = glm::rotate(H_Matrix, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(-0.3f, 0.2f, -0.3f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.2f, 2.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

}

Player::~Player()
{
}
