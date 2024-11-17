#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>

#include <mmsystem.h>
#include <Digitalv.h>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

#include <gl/glm/glm.hpp>
#include <gl/glm/ext.hpp>
#include <gl/glm/gtc/matrix_transform.hpp>
#include <gl/glm/fwd.hpp>

#pragma comment(lib,"winmm.lib")

#define width 1200
#define height 800

#define Engin "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/engins.mp3"
#define SOUND_FILE_NAME_MIS "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/missile.wav"