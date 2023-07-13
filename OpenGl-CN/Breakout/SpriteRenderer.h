#pragma once

#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

class SpriteRenderer {
private:
	Shader shader;
	GLuint quadVAO;

	void InitRendererData();
public:
	SpriteRenderer(const Shader& shader);
	~SpriteRenderer();

	void DrawSprite(const Texture2D& texture, glm::vec2 position,
		glm::vec2 size = glm::vec2(10.0f, 10.0f), GLfloat rotate = 0.0f,
		glm::vec3 color = glm::vec3(1.0f));
};