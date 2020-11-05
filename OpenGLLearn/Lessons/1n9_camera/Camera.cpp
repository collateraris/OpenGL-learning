#include "Camera.h"

#include <GLFW\glfw3.h>

#include <algorithm>

using namespace lesson_1n9;

CCamera& CCamera::Get()
{
    static CCamera camera;
    return camera;
}


void CCamera::KeyProcessing(int key, int action)
{
    if (action == GLFW_PRESS)
        keys[key] = bPRESS;
    else if (action == GLFW_RELEASE)
        keys[key] = bRELEASE;
}

void CCamera::MouseProcessing(float xpos, float ypos)
{
    static GLfloat lastX = 400.0f;
    static GLfloat lastY = 300.0f;

    static GLfloat yaw = -90.0f;
    static GLfloat pitch = 0.0f;
    static bool bFirstMouse = true;

    if (bFirstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        bFirstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos; // Note that we convert the angle to radians first
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;
    pitch = std::clamp(pitch, -89.0f, 89.0f);

    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    this->mCameraFront = glm::normalize(front);
}

void CCamera::ScrollProcessing(float xoffset, float yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

void CCamera::Movement(float deltaTime)
{
    GLfloat cameraSpeed = 5.0f * deltaTime;
    if (keys[GLFW_KEY_W])
        mCameraPos += cameraSpeed * mCameraFront;
    if (keys[GLFW_KEY_S])
        mCameraPos -= cameraSpeed * mCameraFront;
    if (keys[GLFW_KEY_A])
        mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed;
    if (keys[GLFW_KEY_D])
        mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * cameraSpeed;
}


glm::mat4 CCamera::GetView()
{
	return glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}

float CCamera::GetFov()
{
    return this->fov;
}

const glm::vec3& CCamera::GetCameraPosition() const
{
    return this->mCameraPos;
}

const glm::vec3& CCamera::GetCameraDirection() const
{
    return GetCameraFront();
}

void CCamera::SetCameraPosition(const glm::vec3& pos)
{
    this->mCameraPos = pos;
}

const glm::vec3& CCamera::GetCameraFront() const
{
    return this->mCameraFront;
}

void CCamera::SetCameraFront(const glm::vec3& dir)
{
    this->mCameraFront = dir;
}

const glm::vec3& CCamera::GetCameraUp() const
{
    return this->mCameraUp;
}
