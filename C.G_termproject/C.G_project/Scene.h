#pragma once
#include "Player.h"
#include "Camera.h"
#include "ServerToClient.h"

#define MAX_BULLETS 30  // ��� ���� �߰�

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

    // Ű �Է� ó��
    std::unordered_map<unsigned char, bool> keyStates;
    void KeyDown(unsigned char key) { 
        keyStates[key] = true;

        if (key == 'm') {
            if (!isReady) {
                cout << "�غ� ���� ���� �õ�..." << '\n';  // ������ ��� �߰�
                SendReadyClientToServer();
            }
            else {
                cout << "�غ� ���� ���� ���� �õ�..." << '\n';  // ������ ��� �߰�
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


        // �Ѿ� �߻� ó��
        if (keyStates[' ']) { // space Ű�� ������ ��
            for (auto obj : gameObjects) {
                Bullet* bullet = dynamic_cast<Bullet*>(obj);
                if (bullet && !bullet->active) {
                    bullet->SetPosition(player->GetPosition()); // �߻� ��ġ ����
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

