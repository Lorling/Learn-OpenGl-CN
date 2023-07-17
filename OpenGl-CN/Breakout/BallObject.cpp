#include "BallObject.h"

BallObject::BallObject() :
	GameObject(), Radius(12.5f), Stuck(GL_TRUE), Sticky(GL_FALSE), PassThrough(GL_FALSE)
{
}

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite, GLboolean stuck) :
	GameObject(pos,glm::vec2(radius * 2,radius * 2),sprite,glm::vec3(1.0f),velocity), Radius(radius), Stuck(stuck), Sticky(GL_FALSE), PassThrough(GL_FALSE)
{
}

glm::vec2 BallObject::Move(GLfloat dt, GLuint window_width)
{
	if (Stuck) return Position;
	Position += Velocity * dt;
	//������Ƿ񵽱߽�����
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
	Color = glm::vec3(1.0f);
	Stuck = GL_TRUE;
	PassThrough = GL_FALSE;
	Sticky = GL_FALSE;
}
