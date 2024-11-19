#include "GameFramework.h"

void GameFramework::Initialize(int argc, char** argv) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("SkyLine");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initialization Failed" << std::endl;
        exit(-1);
    }

    scene = Scene::GetInstance();
    scene->Initialize(new Player());
    
}

void GameFramework::TimerCallback(int value)
{
    float currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
    Scene::GetInstance()->Update(currentTime);
    glutPostRedisplay();
    glutTimerFunc(16, TimerCallback, 0); // 타이머를 다시 등록
}

void GameFramework::Run() 
{
    glutDisplayFunc([]() { Scene::GetInstance()->Render(); });
    glutReshapeFunc([](int w, int h) { Scene::GetInstance()->Resize(w, h); });
    glutKeyboardFunc([](unsigned char key, int x, int y) { Scene::GetInstance()->HandleKeyboard(key, true); });
    glutKeyboardUpFunc([](unsigned char key, int x, int y) { Scene::GetInstance()->HandleKeyboard(key, false); });
        
    glutTimerFunc(16, TimerCallback, 0);

    glutMainLoop();
}

void GameFramework::Shutdown() 
{
    delete scene;
}

// test;;