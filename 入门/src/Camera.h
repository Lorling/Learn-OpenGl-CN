#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.f;
const float PITCH = 0.0f;
const float FOV = 45.0f;
const float SPEED = 1.5f;
const float SENSITIVITY = 0.2f;

class Camera {
private:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 WorldUp;
	glm::vec3 Up;
	glm::vec3 Right;

	float Yaw;
	float Pitch;
	float Fov;
	float MovementSpeed;
	float MouseSensitivity;

	bool firstMouse = true;
	float lastx, lasty;
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float posx, float posy, float posz, float upx, float upy, float upz, float yaw, float pitch);

	glm::mat4 GetViewMatrix();
	void ProcessKeyborad(Camera_Movement direction, float deltaTime);//���̻ص�����
	void MouseMovement(double xpos, double ypos);//���ص�����
	void MoustScroll(double yoffset);//���ֻص�����

	float GetFov() { return Fov; }
private:
	void UpdateCameraVectors();
};