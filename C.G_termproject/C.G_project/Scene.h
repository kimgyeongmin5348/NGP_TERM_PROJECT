#pragma once
#include "Player.h"
#include "Camera.h"
#include "ServerToClient.h"

#define MAX_BULLETS 30  // 상수 정의 추가

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
    //void HandleKeyboard(unsigned char key, bool isPressed);

    void AddGameObject(Object* obj) {
        gameObjects.push_back(obj);
    }

    // 키 입력 처리
    std::unordered_map<unsigned char, bool> keyStates;
    void KeyDown(unsigned char key) { 
        keyStates[key] = true;

        if (key == 'm') {
            if (!isReady) {
                cout << "준비 상태 전송 시도..." << '\n';  // 디버깅용 출력 추가
                SendReadyClientToServer();
            }
            else {
                cout << "준비 해제 상태 전송 시도..." << '\n';  // 디버깅용 출력 추가
                SendNotReadyClientToServer();
            }
        }

        if (keyStates['q']) exit(0);
        if (keyStates['w']) { player->state = UP; }
        if (keyStates['a']) { player->state = LEFT; }
        if (keyStates['s']) { player->state = DOWN; }
        if (keyStates['d']) { player->state = RIGHT; }
        // if (keyStates['m'] || keyStates['M']) { player->state = ARE_YOU_READY; }
        //std::cerr << player->GetPosition().x << ", " << player->GetPosition().y << ", " << player->GetPosition().z << std::endl;


        // 총알 발사 처리
        if (keyStates[' ']) { // space 키가 눌렸을 때
            for (auto obj : gameObjects) {
                Bullet* bullet = dynamic_cast<Bullet*>(obj);
                if (bullet && !bullet->active) {
                    bullet->SetPosition(player->GetPosition()); // 발사 위치 설정
                    bullet->active = true;
                    break;
                }
            }
        }

    }
    void KeyUp(unsigned char key) { 
        keyStates[key] = false;
        player->state = 999;
    }


public:
    GLuint VAO[3], VBO[6];
    GLuint s_program;

    GLuint shaderID;
    GLuint vertexShader;
    GLuint fragmentShader;

    GLint g_window_w, g_window_h;

public:
    GLchar* filetobuf(const GLchar* file);
    virtual GLvoid InitBuffer();

    void make_vertexShaders();
    void make_fragmentShader();
    void InitShader();

private:
    Player* player;
    //Building** buildings;
    std::vector<Object*> gameObjects;
    Camera* camera;
    bool isReady = false;
};

