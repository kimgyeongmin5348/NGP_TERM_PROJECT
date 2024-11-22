#include "stdafx.h"
#include "Scene.h"

float bottom[] =
{
    1, 0, -1,
    -1, 0, 1,
    1, 0, 1,
    1, 0, -1,
    -1, 0, -1,
    -1, 0, 1
};

Scene::Scene()
{
}

Scene::~Scene()
{
}

GLchar* Scene::filetobuf(const GLchar* file)
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

GLvoid Scene::InitBuffer()
{
}

void Scene::make_vertexShaders()
{
    GLchar* vertexSource;
    vertexSource = filetobuf("vertex.glsl");
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);
}

void Scene::make_fragmentShader()
{
    GLchar* fragmentSource;
    fragmentSource = filetobuf("fragment.glsl");
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
}

void Scene::InitShader()
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

void Scene::Initialize()
{
    InitShader();
    BuildObject();
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);

}
    
void Scene::BuildObject()
{
    // 플레이어, Ground, 빌딩
    player = new Player(this);
    player->state = 999;

    //gameObjects.push_back(player);
    //player->SetPosition(glm::vec3(0.0f, 5.0f, 0.0f));

    Ground* ground = new Ground();
    gameObjects.push_back(ground);

    for (int i = 0; i < 25; ++i) {
        Bullet* bullet = new Bullet();
        gameObjects.push_back(bullet);
    }

    for (int i = 0; i < 100; ++i) {
        for (int j = 0; j < 10; ++j) {
            Building* building = new Building();
            building->Setting(j);
            gameObjects.push_back(building);
        }
    }
}

void Scene::Render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 검사 (클리핑)
	glUseProgram(s_program);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 cameraPos = glm::vec3(player->GetPosition().x, player->GetPosition().y, player->GetPosition().z - 0.3f);
    glm::vec3 cameraDirection = player->GetPosition();
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    view = glm::rotate(view, glm::radians(-20.f), glm::vec3(1.0f, 0.0f, 0.0f));
    //view = glm::rotate(view, glm::radians(camera.y_rotate_aoc), glm::vec3(0.0f, 1.0f, 0.0f));
    //view = glm::translate(view, glm::vec3(player->GetPosition().x, 0.0f, player->GetPosition().z));

    projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));

    unsigned int viewLocation = glGetUniformLocation(s_program, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);


    glm::vec4 lightPosition(0.f, 10.f, 20.f, 1.0f);
    lightPosition = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * lightPosition;
    
    unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
    glUniform3f(lightPosLocation, lightPosition.x, lightPosition.y, lightPosition.z);
    
    unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
    glUniform3f(lightColorLocation, 0.7f, 0.7f, 0.7f);

     player->Render(s_program);

    for (auto obj : gameObjects) {
        obj->Render(s_program, 0);
    }
        
	glutSwapBuffers();
	glutPostRedisplay();
}

void Scene::Update(float deltaTime)
{
    player->Update(deltaTime);

    for (auto obj : gameObjects) {
        obj->Update(deltaTime);
    }
}

void Scene::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
}