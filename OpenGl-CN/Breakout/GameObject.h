#pragma once

#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
	glm::vec2 Position, Size, Velocity;//位置，大小，速度
	glm::vec3 Color;//颜色
	GLfloat Rotation;//旋转
	GLboolean IsSolid;//是否坚硬
	GLboolean Destroyed;//是否被摧毁

	Texture2D Sprite;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	//绘制图形
	virtual void Draw(SpriteRenderer& renderer);

	void SetColor(glm::vec3 color);
	void SetSprite(Texture2D sprite);
};

