//#include "stdafx.h"
#define _CRT_SECURE_NO_WARNINGS

#define Engin "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/engins.mp3"
#define SOUND_FILE_NAME_MIS "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/missile.wav"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/fwd.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>

#pragma comment(lib,"winmm.lib")

#include <mmsystem.h>
#include <Digitalv.h>

MCI_OPEN_PARMS openBgm;     // MCI_OPEN_PARMS는 MCI_OPEN 커맨드를 위한 정보가 담긴 구조체
MCI_PLAY_PARMS playBgm;
MCI_OPEN_PARMS openShuffleSound;
MCI_PLAY_PARMS playShuffleSound;

int dwID;

#define WIDTH 1200
#define HEIGHT 800
#define h_vertex 0.2f
#define pi 3.141592
using namespace std;
auto start_time = chrono::high_resolution_clock::now();

GLUquadricObj* qobj;
GLuint s_program;
GLuint VAO[3], VBO[6];

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> random_color(0.1, 1);

// 콘솔에 cout을 통해 디버깅을 용이하게 하기 위함
bool test = true;

GLvoid drawScene();
GLvoid Reshape(int, int);
GLvoid KeyBoard(unsigned char, int, int);
GLvoid KeyBoardUp(unsigned char, int, int);
GLvoid SpecialKeyBoard(int, int, int);
GLvoid SpecialKeyBoardUp(int, int, int);
GLvoid FrameAdvance(int);
GLvoid SetBuildings(int);

float bottom[] =
{
    1, 0, -1,
    -1, 0, 1,
    1, 0, 1,
    1, 0, -1,
    -1, 0, -1,
    -1, 0, 1
};

typedef struct object {
    float x_trans{}, y_trans{}, z_trans{};
    float x_trans_aoc{}, y_trans_aoc{}, z_trans_aoc{};
    float x_rotate{}, y_rotate{}, z_rotate{};
    float x_rotate_aoc{}, y_rotate_aoc{}, z_rotate_aoc{};
    float x_scale{}, y_scale{}, z_scale{};
    float x_scale_aoc{}, y_scale_aoc{}, z_scale_aoc{};
    float color_r{}, color_g{}, color_b{};


    float r{};
}O;

typedef struct function {
    bool x_is_trans{ false }, y_is_trans{ false }, z_is_trans{ false };
    int x_max{}, z_max{};

    bool left_walk{ false };
    bool right_walk{ false };
    bool front_walk{ false };
    bool back_walk{ false };

    bool left_turn{ false };

    bool first_see{ false };

    bool shoot_bullet{ false };
    bool game_start{ false };
}F;

O pilot, build[1000][1000], temp_build[1000][1000], bullet;
O temp, camera;
F h_f, temp_f;

GLuint shaderID;
GLuint vertexShader;
GLuint fragmentShader;

GLenum obj_type{ GL_FILL };
GLint g_window_w, g_window_h;

//struct Camera_Option {
//    float camera_distance = 3.0;
//
//    float mouse_sensitivity = 200;
//
//    float camera_drgree_x{};
//    float camera_drgree_y{};
//    float camera_delta_y = 0;
//
//    float camera_fov = 60.0;
//
//    float camera_moving_timer = 0;
//};
//Camera_Option CO, C1;

//GLvoid Setting();
GLfloat hexa[] = {   //육면체
    //윗면
    -h_vertex, h_vertex, h_vertex,
    h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, -h_vertex,

    h_vertex, h_vertex, h_vertex,
    h_vertex, h_vertex, -h_vertex,
    -h_vertex, h_vertex, -h_vertex,
    //정면
    -h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    h_vertex, -h_vertex, h_vertex,
    h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    //아랫면
    -h_vertex, -h_vertex, h_vertex,
    -h_vertex, -h_vertex, -h_vertex,
    h_vertex, -h_vertex, -h_vertex,

    -h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, -h_vertex,
    h_vertex, -h_vertex, h_vertex,

    //뒷면
    h_vertex, -h_vertex, -h_vertex,
    -h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,

    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,


    //좌측면
    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, -h_vertex,

    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, -h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    //우측면
    h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, h_vertex,

    h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,
    h_vertex, h_vertex, h_vertex
};

GLchar* filetobuf(const GLchar* file)
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

void make_vertexShaders()
{
    GLchar* vertexSource;
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
}
void make_fragmentShader()
{
    GLchar* fragmentSource;
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

}
void InitShader()
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
        cerr << "ERROR :  fragment Shader Fail Compile \n" << errorLog << endl;
        exit(-1);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glUseProgram(s_program);
}
GLvoid InitBuffer()
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


    //glUseProgram(s_program);
    //unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    //glUniform3f(lightPosLocation, 5.0f, 10.0f, 0.0f);
    //unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
    //glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
    //unsigned int objColorLocation = glGetUniformLocation(s_program, "objectColor"); //--- object Color값 전달: (1.0, 0.5, 0.3)의 색
    //glUniform3f(objColorLocation, 0.7f, 0.7f, 0.4f);
}

//GLint Collision(float first_x1, float first_x2, float last_x1, float last_x2)  // i'am 충돌체크에요
//{
//    if (first_x1 <= last_x1 && last_x1 <= first_x2)
//        return 1;
//    if (first_x1 <= last_x2 && last_x2 <= first_x2)
//        return 1;
//    if (last_x1 <= first_x1 && first_x1 <= last_x2)
//        return 1;
//    if (last_x1 <= first_x2 && first_x2 <= last_x2)
//        return 1;
//    return 0;
//}
    
// 플레이어 - 건물 충돌 체크
GLvoid CollisionCheckPB(int i, int j)
{
    if ((build[i][j].x_trans - 0.6f) < pilot.x_trans_aoc && pilot.x_trans_aoc < (build[i][j].x_trans + 0.6f) && pilot.y_trans_aoc < build[i][j].y_scale / 5 - 0.2f) {
        memcpy(&pilot, &temp, sizeof(pilot));
        memcpy(&camera, &temp, sizeof(camera));
        memcpy(&h_f, &temp_f, sizeof(h_f));

        if (test)
            cout << "충돌" << i << "/" << j << " : " << pilot.x_trans_aoc << ", " << pilot.y_trans_aoc << ", " << build[i][j].y_scale / 5 << '\n';
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        double elapsed_seconds = duration.count() * 1e-6;
        std::cerr << "생존 시간: " << elapsed_seconds << " 초!" << std::endl;
        start_time = std::chrono::high_resolution_clock::now();
    }
}

// 총알 - 건물 충돌 체크
GLvoid CollisionCheckBB(int i, int j)
{
    if ((build[i][j].x_trans - 0.6f) < bullet.x_trans_aoc && 
        bullet.x_trans_aoc < (build[i][j].x_trans + 0.6f) && 
        bullet.y_trans_aoc < build[i][j].y_scale / 5 - 0.2f && 
        bullet.z_trans_aoc < (build[i][j].z_trans + 1) && 
        bullet.z_trans_aoc >(build[i][j].z_trans - 1)) {
        
        if(test)
            cout << "총알 충돌" << i << " : " << bullet.x_trans_aoc << ", " << bullet.y_trans_aoc << ", " << build[i][j].y_scale / 5 << "," << bullet.z_trans_aoc << ":::" << build[i][j].z_trans_aoc << '\n';
        
        build[i][j].y_scale = 0;
    }
}

int BUILDING_COUNT = 100;
int BUILDING_COUNT_J = 10;
GLvoid RenderBuilding()
{
    glm::mat4 B_Matrix = glm::mat4(1.0f);
    for (int i = 0; i < BUILDING_COUNT; ++i) {
        for (int j = 0; j < BUILDING_COUNT_J; ++j) {
            glm::mat4 B_Matrix = glm::mat4(1.0f);
            // B_Matrix = glm::translate(B_Matrix, glm::vec3(build[i][j].x_trans, 0.f, build[i][j].z_trans));
            B_Matrix = glm::translate(B_Matrix, glm::vec3(build[i][j].x_trans, 0.f, build[i][j].z_trans));
            B_Matrix = glm::scale(B_Matrix, glm::vec3(2.0f, build[i][j].y_scale, 4.0f));
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
    }
}

GLfloat rot;
GLfloat rot_t;
GLfloat camera_rot;
GLfloat C_R;
GLfloat arm_rot;
GLfloat limit;

GLvoid tRenderPilot() // i'am 헬기(조종사) 에요
{
    // 날개 연결부
    glm::mat4 H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.0f, 1.0f, 0.0f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.5f, 0.2f));
    unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    int objColorLocation = glGetUniformLocation(s_program, "objectColor");
    unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 날개 1
    glm::mat4 H_Matrix1 = glm::mat4(1.0f);
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix1 = glm::rotate(H_Matrix1, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix1 = glm::translate(H_Matrix1, glm::vec3(0.0f, 1.1f, 0.0f));
    H_Matrix1 = glm::scale(H_Matrix1, glm::vec3(4.5f, 0.2f, 0.2f));

    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix1));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 날개 2
    glm::mat4 H_Matrix2 = glm::mat4(1.0f);
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix2 = glm::rotate(H_Matrix2, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));  // only engin ans wings
    H_Matrix2 = glm::translate(H_Matrix2, glm::vec3(0.0f, 1.1f, 0.0f));
    H_Matrix2 = glm::scale(H_Matrix2, glm::vec3(0.2f, 0.2f, 4.5f));

    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix2));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 몸통 중간
    glm::mat4 H_Matrix3 = glm::mat4(1.0f);
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix3 = glm::rotate(H_Matrix3, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix3 = glm::rotate(H_Matrix3, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix3 = glm::translate(H_Matrix3, glm::vec3(0.f, 0.7f, -0.2f));
    H_Matrix3 = glm::scale(H_Matrix3, glm::vec3(1.1f, 1.1f, 3.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix3));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 몸통 앞
    glm::mat4 H_Matrix4 = glm::mat4(1.0f);
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    H_Matrix4 = glm::rotate(H_Matrix4, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));
    H_Matrix4 = glm::rotate(H_Matrix4, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));
    H_Matrix4 = glm::translate(H_Matrix4, glm::vec3(0.f, 0.65f, 0.3f));
    H_Matrix4 = glm::scale(H_Matrix4, glm::vec3(0.3f, 0.3f, 0.4f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix4));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    gluSphere(qobj, 1.0, 20, 30);

    // 몸통 뒤
    glm::mat4 H_Matrix5 = glm::mat4(1.0f);
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix5 = glm::rotate(H_Matrix5, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix5 = glm::rotate(H_Matrix5, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix5 = glm::translate(H_Matrix5, glm::vec3(0.f, 0.7f, -1.0f));
    H_Matrix5 = glm::scale(H_Matrix5, glm::vec3(0.2f, 0.2f, 1.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix5));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    gluCylinder(qobj, 0.3f, 1.1f, 0.5, 100, 1);

    // 몸통 뒤(꼬리앞)
    glm::mat4 H_Matrix6 = glm::mat4(1.0f);
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix6 = glm::rotate(H_Matrix6, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix6 = glm::rotate(H_Matrix6, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix6 = glm::translate(H_Matrix6, glm::vec3(0.f, 0.7f, -1.7f));
    H_Matrix6 = glm::scale(H_Matrix6, glm::vec3(0.2f, 0.2f, 1.0f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix6));
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    gluCylinder(qobj, 0.3f, 0.3f, 1.5, 100, 1);

    // 몸통 뒤(꼬리 날개)
    glm::mat4 H_Matrix7 = glm::mat4(1.0f);
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));  // all
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));  // all
    H_Matrix7 = glm::rotate(H_Matrix7, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix7 = glm::rotate(H_Matrix7, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix7 = glm::translate(H_Matrix7, glm::vec3(0.f, 0.8f, -1.6f));
    H_Matrix7 = glm::scale(H_Matrix7, glm::vec3(0.1f, 0.7f, 0.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix7));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //본체 바닥(왼쪽)
    H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));
    H_Matrix = glm::translate(H_Matrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.3f, 0.2f, -0.3f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.2f, 2.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    //본체 바닥2
    H_Matrix = glm::mat4(1.0f);
    H_Matrix = glm::translate(H_Matrix, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));
    H_Matrix = glm::translate(H_Matrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));  // all
    H_Matrix = glm::rotate(H_Matrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));  // all
    H_Matrix = glm::translate(H_Matrix, glm::vec3(-0.3f, 0.2f, -0.3f));
    H_Matrix = glm::scale(H_Matrix, glm::vec3(0.2f, 0.2f, 2.5f));
    StransformLocation = glGetUniformLocation(s_program, "transform");
    glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(H_Matrix));
    //qobj = gluNewQuadric();
    //gluQuadricDrawStyle(qobj, obj_type);
    objColorLocation = glGetUniformLocation(s_program, "objectColor");
    isCheck = glGetUniformLocation(s_program, "isCheck");
    glUniform1f(isCheck, false);
    glUniform3f(objColorLocation, 0.5f, 0.5f, 0.5f);
    //glBindVertexArray(VAO[0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void SetTransformAndDraw(const glm::mat4& matrix, GLuint program, GLuint vao, const glm::vec3& color, bool isCheck) 
{
    GLuint transformLoc = glGetUniformLocation(program, "transform");
    GLuint colorLoc = glGetUniformLocation(program, "objectColor");
    GLuint checkLoc = glGetUniformLocation(program, "isCheck");

    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform3f(colorLoc, color.r, color.g, color.b);
    glUniform1f(checkLoc, isCheck);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void RenderPart(const glm::mat4& baseMatrix, const glm::vec3& translate, const glm::vec3& scale,
                GLuint program, GLuint vao, const glm::vec3& color, bool isCheck)
{
    glm::mat4 matrix = glm::translate(baseMatrix, translate);
    matrix = glm::scale(matrix, scale);
    SetTransformAndDraw(matrix, program, vao, color, isCheck);
}

void RenderRotatingPart(const glm::mat4& baseMatrix, const glm::vec3& translate, const glm::vec3& scale,
                        const glm::vec3& rotationAxis, float angle, GLuint program, GLuint vao,
                        const glm::vec3& color, bool isCheck) 
{
    glm::mat4 matrix = glm::translate(baseMatrix, translate);
    matrix = glm::rotate(matrix, glm::radians(angle), rotationAxis); // 회전 적용
    matrix = glm::scale(matrix, scale);
    SetTransformAndDraw(matrix, program, vao, color, isCheck);
}

void RenderPilot() {
    // 기본 변환 행렬 (헬기 전체)
    glm::mat4 baseMatrix = glm::mat4(1.0f);
    baseMatrix = glm::translate(baseMatrix, glm::vec3(0.f, pilot.y_trans_aoc, pilot.z_trans_aoc));
    baseMatrix = glm::translate(baseMatrix, glm::vec3(pilot.x_trans_aoc, 0.f, 0.f));
    baseMatrix = glm::rotate(baseMatrix, glm::radians(pilot.x_rotate), glm::vec3(1.0f, 0.f, 0.f));
    baseMatrix = glm::rotate(baseMatrix, glm::radians(pilot.z_rotate), glm::vec3(0.f, 0.f, 1.0f));
    //baseMatrix = glm::rotate(baseMatrix, glm::radians(pilot.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));

    // VAO와 프로그램, 기본 색상
    GLuint vao = VAO[0];
    GLuint program = s_program;
    glm::vec3 defaultColor = glm::vec3(0.5f, 0.5f, 0.5f);

    // 헬기 본체
    RenderPart(baseMatrix, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.2f, 0.5f, 0.2f), program, vao, defaultColor, false);

    // 회전하는 날개 1
    RenderRotatingPart(baseMatrix, glm::vec3(0.0f, 1.1f, 0.0f), glm::vec3(4.5f, 0.2f, 0.2f),
                        glm::vec3(0.0f, 1.0f, 0.0f), pilot.y_rotate_aoc, program, vao, defaultColor, false);

    // 회전하는 날개 2
    RenderRotatingPart(baseMatrix, glm::vec3(0.0f, 1.1f, 0.0f), glm::vec3(0.2f, 0.2f, 4.5f),
                        glm::vec3(0.0f, 1.0f, 0.0f), pilot.y_rotate_aoc, program, vao, defaultColor, false);

    // 기타 부품 (몸통, 꼬리 등)
    RenderPart(baseMatrix, glm::vec3(0.f, 0.7f, -0.2f), glm::vec3(1.1f, 1.1f, 3.0f), program, vao, defaultColor, false); // 몸통 중간
    RenderPart(baseMatrix, glm::vec3(0.f, 0.65f, 0.3f), glm::vec3(0.3f, 0.3f, 0.4f), program, vao, defaultColor, false); // 몸통 앞
    RenderPart(baseMatrix, glm::vec3(0.f, 0.7f, -1.0f), glm::vec3(0.2f, 0.2f, 1.0f), program, vao, defaultColor, false); // 몸통 뒤
    RenderPart(baseMatrix, glm::vec3(0.f, 0.7f, -1.7f), glm::vec3(0.2f, 0.2f, 1.0f), program, vao, defaultColor, false); // 꼬리 앞
    RenderPart(baseMatrix, glm::vec3(0.f, 0.8f, -1.6f), glm::vec3(0.1f, 0.7f, 0.5f), program, vao, defaultColor, false); // 꼬리 날개
    RenderPart(baseMatrix, glm::vec3(0.3f, 0.2f, -0.3f), glm::vec3(0.2f, 0.2f, 2.5f), program, vao, defaultColor, false); // 바닥 왼쪽
    RenderPart(baseMatrix, glm::vec3(-0.3f, 0.2f, -0.3f), glm::vec3(0.2f, 0.2f, 2.5f), program, vao, defaultColor, false); // 바닥 오른쪽
}



bool bullet_flag = false;
GLvoid RenderBullet()
{
    if (h_f.shoot_bullet) {
        glm::mat4 RenderBullet = glm::mat4(1.0f);
        if (bullet_flag) {
            bullet.x_trans_aoc = pilot.x_trans_aoc;
            bullet.y_trans_aoc = pilot.y_trans_aoc;
            bullet.z_trans_aoc = pilot.z_trans_aoc;
        }
        bullet_flag = false;

        RenderBullet = glm::translate(RenderBullet, glm::vec3(bullet.x_trans_aoc, bullet.y_trans_aoc, bullet.z_trans_aoc));
        RenderBullet = glm::rotate(RenderBullet, glm::radians(bullet.x_rotate), glm::vec3(1.0f, 0.f, 0.f));
        RenderBullet = glm::rotate(RenderBullet, glm::radians(bullet.z_rotate), glm::vec3(0.f, 0.f, 1.0f));
        // Bullet = glm::translate(Bullet, glm::vec3(0.f, 0.f, bullet.z_trans));
        // Bullet = glm::translate(Bullet, glm::vec3(0.f, 0.65f, 0.3f));
        RenderBullet = glm::scale(RenderBullet, glm::vec3(0.3f, 0.3f, 0.4f));
        unsigned int StransformLocation = glGetUniformLocation(s_program, "transform");
        glUniformMatrix4fv(StransformLocation, 1, GL_FALSE, glm::value_ptr(RenderBullet));
        int objColorLocation = glGetUniformLocation(s_program, "objectColor");
        unsigned isCheck = glGetUniformLocation(s_program, "isCheck");
        qobj = gluNewQuadric();
        gluQuadricDrawStyle(qobj, obj_type);
        glUniform1f(isCheck, false);
        glUniform3f(objColorLocation, 1.0f, 1.0f, 1.0f);
        gluSphere(qobj, 0.2, 20, 30);
    }
}

GLvoid Ground()
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

//void playingBgm(void) {
//    openBgm.lpstrElementName = Engin;
//}

void drawScene()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 검사 (클리핑)
    glUseProgram(s_program);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    for (int i = 0; i < 2; ++i) {
        if (i == 0) {
            glViewport(0, 0, WIDTH, HEIGHT);

            if (!h_f.first_see) { // 3인칭 시점
                glm::vec3 cameraPos = glm::vec3(pilot.x_trans, pilot.y_trans_aoc, pilot.z_trans_aoc - 0.3f);
                glm::vec3 cameraDirection = glm::vec3(pilot.x_trans, pilot.y_trans_aoc, pilot.z_trans_aoc);
                glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

                view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
                view = glm::rotate(view, glm::radians(-20.f), glm::vec3(1.0f, 0.0f, 0.0f));
                view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
                view = glm::translate(view, glm::vec3(-pilot.x_trans - pilot.x_trans_aoc, 0.0f, pilot.z_trans));
            }
            else { // 1인칭 시점
                glm::vec3 cameraPos = glm::vec3(0.f, 0.5f, 5.7f);
                glm::vec3 cameraDirection = glm::vec3(0.0f, 0.5f, 5.95f);
                glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

                view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
                view = glm::rotate(view, glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                view = glm::rotate(view, glm::radians(-pilot.y_rotate), glm::vec3(0.0f, 1.0f, 0.0f));
                view = glm::translate(view, glm::vec3(-pilot.x_trans_aoc, -pilot.y_trans_aoc, -pilot.z_trans_aoc));
            }

            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
            projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
        }
        else { // 미니맵
            glViewport(1050, 550, 150, 150);
            glm::vec3 cameraPos = glm::vec3(pilot.x_trans_aoc, 5.0f, pilot.z_trans);
            glm::vec3 cameraDirection = glm::vec3(pilot.x_trans_aoc, 0.0f, pilot.z_trans);
            glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 10.0f);

            view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
            projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
            projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));
        }

        unsigned int viewLocation = glGetUniformLocation(s_program, "view");
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

        unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

        float lightInitialX = 0;
        float lightInitialY = 10;
        float lightInitialZ = 20.f;
        float lightRotationAngle = glm::radians(0.0f);
        glm::vec4 lightPosition(lightInitialX, lightInitialY, lightInitialZ, 1.0f);
        lightPosition = glm::rotate(glm::mat4(1.0f), lightRotationAngle, glm::vec3(1.0f, 0.0f, 0.0f)) * lightPosition;
        glUseProgram(s_program);
        unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
        glUniform3f(lightPosLocation, lightPosition.x, lightPosition.y, lightPosition.z);
        // glUniform3f(lightPosLocation, 4, 10, 10.f);
        // glUniform3f(lightPosLocation, pilot.x_trans_aoc * 10, pilot.y_trans_aoc * 10, 0.5f);
        unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
        glUniform3f(lightColorLocation, 0.7f, 0.7f, 0.7f);
        // cout << pilot.x_trans_aoc << '\n';

        RenderPilot();
        RenderBullet();
        Ground();
        RenderBuilding();
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

//void Reshape(int w, int h) {
//    g_window_w = w;
//    g_window_h = h;
//    glViewport(0, 0, w, h);
//}

GLvoid KeyBoardUp(unsigned char key, int x, int y) {
    switch (key)
    {
    case 'a':
        h_f.left_walk = false;
        break;
    case 'd':
        h_f.right_walk = false;
        break;
    case 'w':
        h_f.front_walk = false;
        break;
    case 's':
        h_f.back_walk = false;
        break;
    }
    glutPostRedisplay();
}

GLvoid KeyBoard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q':
        exit(0);
        cout << "exit program" << '\n';
        break;
    case 'd':
        h_f.right_walk = true;
        break;
    case 'a':
        h_f.left_walk = true;
        break;
    case 'w':
        h_f.front_walk = true;
        break;
    case 's':
        h_f.back_walk = true;
        break;
    case 'm':
        h_f.game_start = !h_f.game_start;
        break;
    case 'c':
        memcpy(&pilot, &temp, sizeof(pilot));
        memcpy(&camera, &temp, sizeof(camera));
        memcpy(&h_f, &temp_f, sizeof(h_f));
        break;
    case '1':
        h_f.first_see = true;
        // cout << "1인칭" << endl;
        break;
    case '3':
        h_f.first_see = false;
        // cout << "3인칭" << endl;
        break;
    case ' ':
        h_f.shoot_bullet = true;
        bullet_flag = true;
        // PlaySound(TEXT(SOUND_FILE_NAME_MIS), NULL, SND_ASYNC);
        break;
    }
    glutPostRedisplay();
}

void main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("SkyLine");

    //PlaySound(TEXT(SOUND_FILE_NAME_PLAY), NULL, SND_ASYNC | SND_LOOP);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        cerr << "NOT INIT" << '\n';

    InitShader();
    InitBuffer();
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);

    glutDisplayFunc(drawScene);

    glutKeyboardFunc(KeyBoard);
    glutKeyboardUpFunc(KeyBoardUp);

    glutTimerFunc(5, FrameAdvance, 1);

    glutTimerFunc(1000, SetBuildings, 1);
    RenderBuilding();

    glutMainLoop();
}

GLvoid FrameAdvance(int value)
{
    if (h_f.x_is_trans) {
        pilot.x_trans += 0.1;
    }

    if (h_f.right_walk) {
        if (pilot.x_trans_aoc > -4)   // pilot이 최대 움직일 수 있는 공간
            pilot.x_trans_aoc -= 0.05f;
        if (pilot.z_rotate < 15)
            pilot.z_rotate += 1.0f;

    }
    else if (h_f.left_walk) {
        if (pilot.x_trans_aoc < 4)    // pilot이 최대 움직일 수 있는 공간
            pilot.x_trans_aoc += 0.05f;
        if (pilot.z_rotate > -15)
            pilot.z_rotate -= 1.0f;
    }
    else if (h_f.back_walk) {
        if (pilot.y_trans_aoc > 0)      // pilot이 최대 내려갈 수 있는 공간 
            pilot.y_trans_aoc -= 0.03f;
        if (pilot.x_rotate > -15)
            pilot.x_rotate -= 1.0f;
    }
    else if (h_f.front_walk) {
        if (pilot.y_trans_aoc < 4)      // pilot이 최대 올라갈 수 있는 공간 
            pilot.y_trans_aoc += 0.03f;
        if (pilot.x_rotate < 15)
            pilot.x_rotate += 1.0f;
    }
    else {
        if (pilot.x_rotate < 0.f)
            pilot.x_rotate += 1.0f;
        if (pilot.x_rotate > 0.f)
            pilot.x_rotate -= 1.0f;
        if (pilot.z_rotate > 0.f)
            pilot.z_rotate -= 1.0f;
        if (pilot.z_rotate < 0.f)
            pilot.z_rotate += 1.0f;
    }

    // 프로펠러 회전
    pilot.y_rotate_aoc += 10;

    // 총알 event
    if (h_f.shoot_bullet)
        bullet.z_trans_aoc += 0.3f;
    if (bullet.z_trans_aoc > 15) {
        bullet.z_trans_aoc = -10;
        h_f.shoot_bullet = false;
        bullet_flag = false;
    }


    if (h_f.game_start) {
        std::uniform_real_distribution<float> random_building_come(0.000001, 0.4);

        // 건물 다가오기
        for (int i = 0; i < BUILDING_COUNT; ++i) {
            for (int j = 0; j < BUILDING_COUNT_J; ++j) {
                build[i][j].z_trans -= random_building_come(gen);
                CollisionCheckBB(i, j);
                if (build[i][j].z_trans < 0.4f && build[i][j].z_trans > -0.1f) {
                    CollisionCheckPB(i, j);
                }

                //if (build[i][j].z_trans < -30.8f) {  // 충돌 감지 해야하는 시점
                    //building_setting_flag = false;
                    // Building_Setting();
                    // build[i][j].z_trans = 40.f;
                 //}
            }
        }

    }

    glutPostRedisplay();
    glutTimerFunc(5, FrameAdvance, 1);

}

bool building_flag = false;
std::uniform_real_distribution<float> random_building_x_pos(-20, 20);
std::uniform_real_distribution<float> random_building_hight(1, 25);
GLvoid SetBuildings(int value)
{
    if (h_f.game_start) {
        for (int i = 0; i < BUILDING_COUNT; ++i) {
            build[i][value].x_trans = random_building_x_pos(gen);
            build[i][value].y_trans = 0;
            build[i][value].y_scale = random_building_hight(gen);
            build[i][value].z_trans = 40.0f;

            /*cout << "빌딩 생성 위치 [" << i << "]" << '\n';
            cout << "[x] : " << build[i][j].x_trans << '\n';
            cout << "[y_scale] : " << build[i][j].y_scale << '\n';*/
        }
        value++;
        if (value > BUILDING_COUNT_J)
            value = 0;
    }
    else if (!building_flag) {
        for (int i = 0; i < BUILDING_COUNT; ++i) {
            for (int j = 0; j < 1; ++j) {
                build[i][j].x_trans = random_building_x_pos(gen);
                build[i][j].y_trans = 0;
                build[i][j].y_scale = random_building_hight(gen);
                build[i][j].z_trans = 40.0f;

                //cout << "빌딩 생성 위치 [" << i << "]" << '\n';
                //cout << "[x] : " << build[i][j].x_trans << '\n';
                //cout << "[y_scale] : " << build[i][j].y_scale << '\n';
            }
        }
        building_flag = true;
    }

    glutTimerFunc(1000, SetBuildings, value);
}