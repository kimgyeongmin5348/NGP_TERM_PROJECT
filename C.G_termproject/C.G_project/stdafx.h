#define _CRT_SECURE_NO_WARNINGS //--- ÇÁ·Î±×·¥ ¸Ç ¾Õ¿¡ ¼±¾ðÇÒ °Í
#define Engin "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/engins.mp3"
#define SOUND_FILE_NAME_MIS "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/missile.wav"
#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <string>
#include <glm/fwd.hpp>
#include <chrono>
#pragma comment(lib,"winmm.lib")
#include <mmsystem.h>
#include <Digitalv.h>

#define width 1200
#define height 800

#define h_vertex 0.2f

float bottom[] =
{
    1, 0, -1,
    -1, 0, 1,
    1, 0, 1,
    1, 0, -1,
    -1, 0, -1,
    -1, 0, 1
};

GLfloat hexa[] = {   //À°¸éÃ¼
    //À­¸é
    -h_vertex, h_vertex, h_vertex,
    h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, -h_vertex,

    h_vertex, h_vertex, h_vertex,
    h_vertex, h_vertex, -h_vertex,
    -h_vertex, h_vertex, -h_vertex,
    //Á¤¸é
    -h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    h_vertex, -h_vertex, h_vertex,
    h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    //¾Æ·§¸é
    -h_vertex, -h_vertex, h_vertex,
    -h_vertex, -h_vertex, -h_vertex,
    h_vertex, -h_vertex, -h_vertex,

    -h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, -h_vertex,
    h_vertex, -h_vertex, h_vertex,

    //µÞ¸é
    h_vertex, -h_vertex, -h_vertex,
    -h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,

    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,


    //ÁÂÃø¸é
    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, h_vertex, h_vertex,
    -h_vertex, h_vertex, -h_vertex,

    -h_vertex, -h_vertex, -h_vertex,
    -h_vertex, -h_vertex, h_vertex,
    -h_vertex, h_vertex, h_vertex,

    //¿ìÃø¸é
    h_vertex, -h_vertex, h_vertex,
    h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, h_vertex,

    h_vertex, -h_vertex, -h_vertex,
    h_vertex, h_vertex, -h_vertex,
    h_vertex, h_vertex, h_vertex
};
