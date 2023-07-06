#include "Camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
{
	Position = position;
	WorldUp = up;
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

Camera::Camera(float posx, float posy, float posz, float upx, float upy, float upz, float yaw, float pitch) :
	Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Fov(FOV)
{
	Position = glm::vec3(posx, posy, posz);
	WorldUp = glm::vec3(upx, upy, upz);
	Yaw = yaw;
	Pitch = pitch;
	UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyborad(Camera_Movement direction, float deltaTime)
{
    //如果用户按下WSAD，就将摄像机向相应方向移动
    float cameraSpeed = MovementSpeed * deltaTime;//摄像机移动速度
    if (direction == FORWARD) {
        Position += cameraSpeed * Front;
    }
    if (direction == BACKWARD) {
        Position -= cameraSpeed * Front;
    }
    if (direction == LEFT) {
        Position -= cameraSpeed * Right;
    }
    if (direction == RIGHT) {
        Position += cameraSpeed * Right;
    }
}

void Camera::MouseMovement(double xpos, double ypos)
{
    if (firstMouse) {
        firstMouse = false;
        lastx = xpos;
        lasty = ypos;
    }
    float xoffset = xpos - lastx;
    float yoffset = lasty - ypos;// 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    lastx = xpos;
    lasty = ypos;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;
    Yaw += xoffset;
    Pitch += yoffset;
    //限制摄像机不能俯仰九十度
    if (Pitch > 89.0f) Pitch = 89.0f;
    if (Pitch < -89.0f) Pitch = -89.0f;

    UpdateCameraVectors();
}

void Camera::MoustScroll(double yoffset)
{
    if (Fov >= 1.0f && Fov <= 45.0f) {
        Fov += yoffset;
    }
    if (Fov > 45.0f) Fov = 45.0f;
    if (Fov < 1.0f) Fov = 1.0f;
}

void Camera::UpdateCameraVectors()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	Front = glm::normalize(front);
    
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
