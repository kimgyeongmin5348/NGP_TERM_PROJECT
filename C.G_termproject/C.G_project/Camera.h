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
    glm::vec3 position;      // ī�޶� ��ġ
    glm::vec3 target;        // ī�޶� �ٶ󺸴� ���
    glm::vec3 up;           // ī�޶� ���� ����
    float distance;         // Ÿ�����κ����� �Ÿ�
    float pitch;           // ���� ȸ����
    float yaw;            // �¿� ȸ����
};

