#include "GameFramework.h"

void GameFramework::Initialize(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("SkyLine");

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW Initialization Failed" << std::endl;
        exit(-1);
    }

    scene = new Scene();
    scene->Initialize();
}

void GameFramework::Run() {

    glutDisplayFunc([]() { Scene::GetInstance()->Render(); });
    glutReshapeFunc([](int w, int h) { Scene::GetInstance()->Resize(w, h); });
    glutKeyboardFunc([](unsigned char key, int x, int y) { Scene::GetInstance()->HandleKeyboard(key, true); });
    glutKeyboardUpFunc([](unsigned char key, int x, int y) { Scene::GetInstance()->HandleKeyboard(key, false); });
    glutTimerFunc(16, [](int value) {
        float currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
        Scene::GetInstance()->Update(currentTime);
        }, 0);

    glutMainLoop();
}

void GameFramework::Shutdown() {
    delete scene;
}
