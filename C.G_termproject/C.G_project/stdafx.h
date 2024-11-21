#pragma once
#define _CRT_SECURE_NO_WARNINGS

#define Engin "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/engins.mp3"
#define SOUND_FILE_NAME_MIS "C:/Users/kimkh/Desktop/C.G_test/C.G_project/C.G_project/sound/missile.wav"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <chrono>
#include <unordered_map>

#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>

//#include <gl/glm/glm.hpp>
//#include <gl/glm/ext.hpp>
//#include <gl/glm/gtc/matrix_transform.hpp>
//#include <gl/glm/fwd.hpp>
//#include <gl/glm/gtx/io.hpp>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtx/io.hpp>

#pragma comment(lib,"winmm.lib")

#include <mmsystem.h>
#include <Digitalv.h>

#define WIDTH 1200
#define HEIGHT 800

#define h_vertex 0.2f

using namespace std;

static GLfloat hexa[] = {   //À°¸éÃ¼
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