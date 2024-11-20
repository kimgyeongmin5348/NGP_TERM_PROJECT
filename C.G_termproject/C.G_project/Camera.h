#pragma once
#include "stdafx.h"

#define FIRST_PERSON_CAMERA			0x01
#define THIRD_PERSON_CAMERA			0x03

class Camera
{
public:
	Camera();
	~Camera();

    void UpdateCameraPosition();

public:
    float camera_distance = 3.0;

    float mouse_sensitivity = 200;

    float camera_drgree_x{};
    float camera_drgree_y{};
    float camera_delta_y = 0;

    float fov = 60.0;

    float camera_moving_timer = 0;

private:
    glm::vec3 position;      // 카메라 위치
    glm::vec3 target;        // 카메라가 바라보는 대상
    glm::vec3 up;           // 카메라 상향 벡터
    float distance;         // 타겟으로부터의 거리
    float pitch;           // 상하 회전각
    float yaw;            // 좌우 회전각
};

