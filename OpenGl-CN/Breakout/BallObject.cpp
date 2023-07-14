#include "BallObject.h"

BallObject::BallObject() :
	GameObject(), Radius(12.5f), Stuck(GL_TRUE)
{
}

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite) :
	GameObject(pos,glm::vec2(radius * 2,radius * 2),sprite,glm::vec3(1.0f),velocity), Radius(radius), Stuck(GL_TRUE)
{
}

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	if (Stuck) return Position;
	Position += Velocity * dt;
	//检测球是否到边界以外
	if (Position.x <= 0) {
		Velocity.x = -Velocity.x;
		Position.x = 0;
	}
	if (Position.x >= window_width - 2 * Radius) {
		Velocity.x = -Velocity.x;
		Position.x = window_width - 2 * Radius;
	}
	if (Position.y <= 0) {
		Velocity.y = -Velocity.y;
		Position.y = 0;
	}
	return Position;
}

void BallObject::Reset(glm::vec2 pos, glm::vec2 velocity)
{
	Position = pos;
	Velocity = velocity;
	Stuck = GL_TRUE;
}
