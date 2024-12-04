#pragma once
#include "Player.h"
#include "Camera.h"
#include "ServerToClient.h"

#define MAX_BULLETS 3

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

    void UpdatePlayerPosition(int playerID, const glm::vec3& newPos);

    void UpdateBuilding(int buildingNum, glm::vec3& scale, const glm::vec3& newPos);
    void UpdateOtherBulletPosition(int bulletNum, const glm::vec3& newPos);
    void ProcessBulletBuildingCollision(int BulletNum, int BuildingNum);
    void ProcessPlayerBuildingCollision(int num);

    void AddGameObject(Object* obj) {
        gameObjects.push_back(obj);
    }

    void KeyDown(unsigned char key);

    void KeyUp(unsigned char key);

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
    OtherPlayer* otherPlayer{ nullptr };  // 다른 클라이언트들의 플레이어
    std::vector<Object*> gameObjects;
    Camera* camera{ nullptr };
    bool isReady{ false };
    std::unordered_map<unsigned char, bool> keyStates;
};