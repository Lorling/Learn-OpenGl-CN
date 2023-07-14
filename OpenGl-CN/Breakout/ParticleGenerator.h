#pragma once

#include "GameObject.h"

#include <vector>

class Particle
{
public:
	glm::vec2 Position;
	glm::vec2 Velocity;
	GLfloat Life;
	glm::vec4 Color;

	Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) {}
};

class ParticleGenerator {
public:
	ParticleGenerator(Shader shader,Texture2D texture,GLuint amount);
	void Update(GLfloat dt, GameObject& object, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f));

	void Draw();
private:
	std::vector<Particle> Particles;
	GLuint amount;

	Shader shader;
	Texture2D texture;
	GLuint VAO;
	void Init();
	GLuint FirstUnusedParticle();
	void RespawnParticle(Particle& particle,GameObject& object,glm::vec2 offset = glm::vec2(0.0f,0.0f));
};