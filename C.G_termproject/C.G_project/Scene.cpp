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
    delete mainPlayer;
    for (auto obj : gameObjects) {
        delete obj;
    }
    delete camera;
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
    mainPlayer = new Player(this);
    mainPlayer->state = 0;

    otherPlayer = new OtherPlayer(this);

    Ground* ground = new Ground();
    gameObjects.push_back(ground);

    for (int i = 0; i < MAX_BULLETS; ++i) {
        Bullet* bullet = new Bullet();
        bullet->num = i;
        gameObjects.push_back(bullet);
    }

    for (int i = 0; i < 100; ++i) {
        Building* building = new Building();
        gameObjects.push_back(building);
    }
}

void Scene::Render()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(s_program);

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    glm::vec3 cameraPos = glm::vec3(mainPlayer->GetPosition().x, mainPlayer->GetPosition().y, mainPlayer->GetPosition().z - 0.3f);
    glm::vec3 cameraDirection = mainPlayer->GetPosition();
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    view = glm::rotate(view, glm::radians(-20.f), glm::vec3(1.0f, 0.0f, 0.0f));

    projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
    projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));

    unsigned int viewLocation = glGetUniformLocation(s_program, "view");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

    glm::vec4 lightPosition(0.f, 10.f, 20.f, 1.0f);
    lightPosition = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * lightPosition;

    unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos");
    glUniform3f(lightPosLocation, lightPosition.x, lightPosition.y, lightPosition.z);

    unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor");
    glUniform3f(lightColorLocation, 0.7f, 0.7f, 0.7f);

    mainPlayer->Render(s_program);
    if (otherPlayer) {
        otherPlayer->Render(s_program);
    }

    for (auto obj : gameObjects) {
        obj->Render(s_program, 0);
    }

    glutSwapBuffers();
    glutPostRedisplay();
}

void Scene::Update(float deltaTime)
{
    mainPlayer->Update(deltaTime);
    if (otherPlayer) {
        otherPlayer->Update(deltaTime);
    }

    for (auto obj : gameObjects) {
        obj->Update(deltaTime);
        Bullet* bullet = dynamic_cast<Bullet*>(obj);
        if (bullet && bullet->active) {  // active한 총알만 확인
            cout << "\n=== 총알 위치 전송 정보 ===" << endl;
            cout << "총알 번호: " << bullet->num << endl;
            cout << "위치: (" << bullet->GetPosition().x << ", "
                << bullet->GetPosition().y << ", "
                << bullet->GetPosition().z << ")" << endl;

            // 총알 위치 정보를 서버로 전송
            PacketBulletMove bulletPacket;
            bulletPacket.size = sizeof(PacketBulletMove);
            bulletPacket.type = PACKET_BULLET_MOVE;
            bulletPacket.pos = bullet->GetPosition();
            bulletPacket.num = bullet->num;

            // 패킷 타입 전송
            char type = PACKET_BULLET_MOVE;
            int retval = send(sock, &type, sizeof(char), 0);
            if (retval == SOCKET_ERROR) {
                cout << "총알 이동 타입 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }

            // 총알 이동 패킷 전송
            retval = send(sock, (char*)&bulletPacket, sizeof(bulletPacket), 0);
            if (retval == SOCKET_ERROR) {
                cout << "총알 이동 패킷 전송 실패: " << WSAGetLastError() << endl;
                continue;
            }

            cout << "총알 위치 정보 전송 완료" << endl;
        }
    }
}

void Scene::Resize(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Scene::UpdatePlayerPosition(int playerID, const glm::vec3& newPos) 
{
    if (otherPlayer) {
        //otherPlayer = new OtherPlayer(this);
        otherPlayer->SetPosition(newPos);
    }
}

void Scene::UpdateBuilding(int buildingNum, glm::vec3& scale, const glm::vec3& newPos) 
{
    if (buildingNum >= 0 && buildingNum < gameObjects.size()) {
        Building* building = dynamic_cast<Building*>(gameObjects[buildingNum]);
        if (building) {
            building->SetPosition(newPos);
            building->SetScale(scale);
        }
    }
}

void Scene::UpdateBulletPosition(int bulletNum, const glm::vec3& newPos)
{
    if (bulletNum >= 0 && bulletNum < MAX_BULLETS) {
        for (auto obj : gameObjects) {
            Bullet* bullet = dynamic_cast<Bullet*>(obj);
            if (bullet && bullet->num == bulletNum) {
                bullet->SetPosition(newPos);
                bullet->active = true;
                break;
            }
        }
    }
}

void Scene::ProcessBulletBuildingCollision(int BulletNum, int BuildingNum)
{
    for (auto obj : gameObjects) {
        Building* building = dynamic_cast<Building*>(gameObjects[BuildingNum]);
        Bullet* bullet = dynamic_cast<Bullet*>(gameObjects[BulletNum]);
        if (bullet)bullet->active = false;
    }
}

void Scene::KeyDown(unsigned char key) 
{
    keyStates[key] = true;

    if (key == 'm') {
        if (!isReady) { 
            cout << "준비 상태 전송 시도..." << '\n';
            SendReadyClientToServer();
        }
    }

    if (keyStates['q']) exit(0);
    if (keyStates['w']) { mainPlayer->state = UP; }
    if (keyStates['a']) { mainPlayer->state = LEFT; }
    if (keyStates['s']) { mainPlayer->state = DOWN; }
    if (keyStates['d']) { mainPlayer->state = RIGHT; }

    if (keyStates[' ']) {
        for (auto obj : gameObjects) {
            Bullet* bullet = dynamic_cast<Bullet*>(obj);
            if (bullet && !bullet->active) {
                bullet->SetPosition(mainPlayer->GetPosition());
                bullet->active = true;
                break;
            }
        }
    }
}


void Scene::KeyUp(unsigned char key) 
{
    keyStates[key] = false;
    mainPlayer->state = 0;
}