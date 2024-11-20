#include "Camera.h"

Camera::Camera()
{
}

Camera::~Camera()
{
}

void Camera::UpdateCameraPosition()
{
    float pitchRad = glm::radians(pitch);
    float yawRad = glm::radians(yaw);

    position = target;
    position.x += distance * cos(yawRad) * cos(pitchRad);
    position.y += distance * sin(pitchRad);
    position.z += distance * sin(yawRad) * cos(pitchRad);
}
