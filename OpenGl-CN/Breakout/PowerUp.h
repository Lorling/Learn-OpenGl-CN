#pragma once

#include "GameObject.h"

#include <vector>

const glm::vec2 SIZE(60, 20);
const glm::vec2 VELOCITY(0.0f, 150.0f);//只会向下掉

enum PROPTYPE {
	SPEED,
	STICKY,
	PASS_THROUGH,
	PAD_SIZE_INCREASE,
	CONFUSE,
	CHAOS
};

class PowerUp : public GameObject
{
public:
	PROPTYPE Type;
	GLfloat Duration;
	GLboolean Activated;

	PowerUp(PROPTYPE type, GLfloat duration, glm::vec3 color, glm::vec2 position, Texture2D texture) :
		GameObject(position, SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated(GL_FALSE)
	{ }
};

