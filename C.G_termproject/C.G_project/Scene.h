#pragma once
#include "Player.h"
#include "Camera.h"
#include "ServerToClient.h"

#define MAX_BULLETS 30

class Scene
{
public:
    Scene();
    ~Scene();

    static Scene* GetInstance() {
        static Scene instance;
        return &instance;
    }

    void Initialize();
    void BuildObject();
    void Render();
    void Update(float deltaTime);
    void Resize(int w, int h);

    void UpdatePlayerPosition(int playerID, const glm::vec3& newPos) {
        if (playerID >= 0 && playerID < 2 && playerID != myID) {
            if (otherPlayers[playerID] == nullptr) {
                otherPlayers[playerID] = new Player();
            }
            otherPlayers[playerID]->SetPosition(newPos);
        }
    }

    void UpdateBuilding(int buildingNum, glm::vec3& scale, const glm::vec3& newPos) {
        if (buildingNum >= 0 && buildingNum < gameObjects.size()) {
            Building* building = dynamic_cast<Building*>(gameObjects[buildingNum]);
            if (building) {
                building->SetPosition(newPos);
                building->SetScale(scale);
            }
        }
    }

    void AddGameObject(Object* obj) {
        gameObjects.push_back(obj);
    }

    void KeyDown(unsigned char key) {
        keyStates[key] = true;

        if (key == 'm') {
            if (!isReady) {
                cout << "준비 상태 전송 시도..." << '\n';
                SendReadyClientToServer();
            }
            else {
                cout << "준비 해제 상태 전송 시도..." << '\n';
                SendNotReadyClientToServer();
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

    void KeyUp(unsigned char key) {
        keyStates[key] = false;
        mainPlayer->state = 999;
    }

public:
    GLuint VAO[3], VBO[6];
    GLuint s_program;
    GLuint shaderID;
    GLuint vertexShader;
    GLuint fragmentShader;
    GLint g_window_w, g_window_h;

    GLchar* filetobuf(const GLchar* file);
    virtual GLvoid InitBuffer();
    void make_vertexShaders();
    void make_fragmentShader();
    void InitShader();

private:
    Player* mainPlayer{ nullptr };  // 현재 클라이언트의 플레이어
    Player* otherPlayers[2]{ nullptr, nullptr };  // 다른 클라이언트들의 플레이어
    std::vector<Object*> gameObjects;
    Camera* camera{ nullptr };
    bool isReady{ false };
    std::unordered_map<unsigned char, bool> keyStates;
};