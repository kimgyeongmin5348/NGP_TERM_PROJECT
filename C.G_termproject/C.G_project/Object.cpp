#include "stdafx.h"
#include "Object.h"

Object::Object()
{
}

Object::~Object()
{
}

GLchar* Object::filetobuf(const GLchar* file)
{
    FILE* fptr;
    long length;
    GLchar* buf;

    fopen_s(&fptr, file, "rb");
    if (!fptr)
        return NULL;

    fseek(fptr, 0, SEEK_END);
    length = ftell(fptr);
    buf = (GLchar*)malloc(length + 1);
    fseek(fptr, 0, SEEK_SET);
    fread(buf, length, 1, fptr);
    fclose(fptr);
    buf[length] = 0;

    return buf;
}

GLvoid Object::InitBuffer()
{
    glGenVertexArrays(3, VAO);

    glBindVertexArray(VAO[0]);
    glGenBuffers(2, &VBO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hexa), hexa, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAO[1]);
    glGenBuffers(2, &VBO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bottom), bottom, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
}

void Object::make_vertexShaders()
{
    GLchar* vertexSource;
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
}

void Object::make_fragmentShader()
{
    GLchar* fragmentSource;
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
}

void Object::InitShader()
{
    make_vertexShaders();
    make_fragmentShader();

    s_program = glCreateProgram();

    glAttachShader(s_program, vertexShader);
    glAttachShader(s_program, fragmentShader);
    glLinkProgram(s_program);

    GLint result;
    GLchar errorLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR :  fragment Shader Fail Compile \n" << errorLog << std::endl;
        exit(-1);
    }
    else
        std::cout << "good" << std::endl;
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(s_program);
}

/////////////////////////////////////////////////////////////////////////

Bullet::Bullet()
{
    glm::mat4 Bullet = glm::mat4(1.0f);
    Bullet = glm::translate(Bullet, glm::vec3(x_trans_aoc, y_trans_aoc, z_trans_aoc));
    Bullet = glm::rotate(Bullet, glm::radians(x_rotate), glm::vec3(1.0f, 0.f, 0.f));
    Bullet = glm::rotate(Bullet, glm::radians(z_rotate), glm::vec3(0.f, 0.f, 1.0f));
    Bullet = glm::scale(Bullet, glm::vec3(0.3f, 0.3f, 0.4f));

    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Bullet));
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
    gluSphere(qobj, 0.2, 20, 30);
}

Bullet::~Bullet()
{
}

/////////////////////////////////////////////////////////////////////////

Building::Building()
{
    glm::mat4 B_Matrix = glm::mat4(1.0f);
    B_Matrix = glm::translate(B_Matrix, glm::vec3(x_trans, 0.f, z_trans));
    B_Matrix = glm::scale(B_Matrix, glm::vec3(2.0f, y_scale, 4.0f));
    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(B_Matrix));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glm::vec4 cubeColor = glm::vec4(1.0f, 0.0f, 0.5f, 1.0f); // 색상을 원하는 값으로 설정
    glUniform3f(objColorLocation, cubeColor.r, cubeColor.g, cubeColor.b);
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

Building::~Building()
{
}

/////////////////////////////////////////////////////////////////////////

Ground::Ground()
{
    glm::mat4 Bottom = glm::mat4(1.0f);
    Bottom = glm::scale(Bottom, glm::vec3(1000.0f, 0.f, 1000.0f));
    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(Bottom));
    qobj = gluNewQuadric();
    gluQuadricDrawStyle(qobj, obj_type);
    unsigned int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.1f, 0.1f, 0.1f);
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}
