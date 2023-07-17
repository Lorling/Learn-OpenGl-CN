#pragma once

#include "GameObject.h"

class BallObject : public GameObject
{
public :
	//球的状态
	GLfloat Radius;
	GLboolean Stuck;//球是否固定在挡板上
	GLboolean Sticky;
	GLboolean PassThrough;


	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite, GLboolean stuck = GL_TRUE);

	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void Reset(glm::vec2 pos, glm::vec2 velocity);
};

