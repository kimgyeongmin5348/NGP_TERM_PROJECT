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

void Scene::Initialize(Player* pPlayer)
{
    player = pPlayer;
    InitShader();
    BuildObject();
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CW);
}
    
void Scene::BuildObject()
{
	// 플레이어, Ground, 빌딩_Mat
    player->SetPosition(glm::vec3(0.0f, 1.0f, -5.0f));

    for (int i = 0; i < 1000; ++i) {
        Building* building = new Building();
        building->Setting();
        gameObjects.push_back(building);
    }
}

void Scene::Render()
{
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 깊이 검사 (클리핑)
	glUseProgram(s_program);

    /*
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	// 미니맵
	glViewport(1050, 550, 150, 150);
	glm::vec3 cameraPos = glm::vec3(player->x_trans_aoc, 5.0f, player->z_trans);
	glm::vec3 cameraDirection = glm::vec3(player->x_trans_aoc, 0.0f, player->z_trans);
	glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 10.0f);

	view = glm::lookAt(cameraPos, cameraDirection, cameraUp);
	projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 50.0f);
	projection = glm::translate(projection, glm::vec3(0.0, 0.0, -5.0));

	unsigned int viewLocation = glGetUniformLocation(s_program, "view");
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

	unsigned int projectionLocation = glGetUniformLocation(s_program, "projection");
	glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);

	// 조명
	float lightInitialX = 0;
	float lightInitialY = 10;
	float lightInitialZ = 20.f;
	float lightRotationAngle = glm::radians(0.0f);
	glm::vec4 lightPosition(lightInitialX, lightInitialY, lightInitialZ, 1.0f);
	lightPosition = glm::rotate(glm::mat4(1.0f), lightRotationAngle, glm::vec3(1.0f, 0.0f, 0.0f)) * lightPosition;
	glUseProgram(s_program);
	unsigned int lightPosLocation = glGetUniformLocation(s_program, "lightPos"); //--- lightPos 값 전달: (0.0, 0.0, 5.0);
	glUniform3f(lightPosLocation, lightPosition.x, lightPosition.y, lightPosition.z);
	unsigned int lightColorLocation = glGetUniformLocation(s_program, "lightColor"); //--- lightColor 값 전달: (1.0, 1.0, 1.0) 백색
	glUniform3f(lightColorLocation, 0.7f, 0.7f, 0.7f);
    */

    player->Render(s_program);

    // 오브젝트 그리기 - 빌딩, Ground 등
    for (auto obj : gameObjects) {
        obj->Render(s_program);
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

void Scene::HandleKeyboard(unsigned char key, bool isPressed)
{
    if (key == 'q') exit(0);
    if (key == ' ') { // 스페이스바로 총알 발사
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
