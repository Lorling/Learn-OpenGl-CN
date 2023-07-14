#pragma once

#include <glm/glm.hpp>

#include "Texture.h"
#include "SpriteRenderer.h"

class GameObject
{
public:
	glm::vec2 Position, Size, Velocity;//λ�ã���С���ٶ�
	glm::vec3 Color;//��ɫ
	GLfloat Rotation;//��ת
	GLboolean IsSolid;//�Ƿ��Ӳ
	GLboolean Destroyed;//�Ƿ񱻴ݻ�

	Texture2D Sprite;

	GameObject();
	GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	//����ͼ��
	virtual void Draw(SpriteRenderer& renderer);

	void SetColor(glm::vec3 color);
	void SetSprite(Texture2D sprite);
};

