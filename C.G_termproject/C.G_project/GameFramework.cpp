#include "GameFramework.h"

void GameFramework::Initialize(int argc, char** argv) 
{
    // 1. GLUT �ʱ�ȭ
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WIDTH, HEIGHT);

    // 2. ������ ����
    glutCreateWindow("SkyLine");

    // 3. �ݹ� �Լ� ��� - Initialize �ܰ迡�� ���� ����
    glutDisplayFunc([]() { Scene::GetInstance()->Render(); });
    glutReshapeFunc([](int w, int h) { Scene::GetInstance()->Resize(w, h); });
    glutKeyboardFunc([](unsigned char key, int x, int y) { Scene::GetInstance()->KeyDown(key); });
    glutKeyboardUpFunc([](unsigned char key, int x, int y) { Scene::GetInstance()->KeyUp(key); });
    glutTimerFunc(16, TimerCallback, 0);

    // 4. GLEW �ʱ�ȭ
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initialization Failed" << std::endl;
        exit(-1);
    }

    // 5. Scene �ʱ�ȭ
    scene = Scene::GetInstance();
    scene->Initialize();
}

void GameFramework::TimerCallback(int value)
{
    float currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    Scene::GetInstance()->Update(currentTime);
    glutPostRedisplay();
    glutTimerFunc(16, TimerCallback, 0); // Ÿ�̸Ӹ� �ٽ� ���
}

void GameFramework::Run() 
{
    glutMainLoop();
}

void GameFramework::Shutdown() 
{
    delete scene;
}

// test;;