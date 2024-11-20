#pragma once
#include "Player.h"
#include "Camera.h"

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
    void KeyDown(unsigned char key) { keyStates[key] = true; }
    void KeyUp(unsigned char key) { keyStates[key] = false; }

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
    std::vector<Object*> gameObjects;
    Camera* camera;
};

