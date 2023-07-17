#pragma once

#include "GameObject.h"

class BallObject : public GameObject
{
public :
	//���״̬
	GLfloat Radius;
	GLboolean Stuck;//���Ƿ�̶��ڵ�����
	GLboolean Sticky;
	GLboolean PassThrough;


	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite, GLboolean stuck = GL_TRUE);

	glm::vec2 Move(GLfloat dt, GLuint window_width);
	void Reset(glm::vec2 pos, glm::vec2 velocity);
};

