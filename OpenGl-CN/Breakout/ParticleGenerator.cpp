#include "ParticleGenerator.h"

#include <iostream>

ParticleGenerator::ParticleGenerator(Shader shader, Texture2D texture, GLuint amount) :
	shader(shader), texture(texture), amount(amount)
{
	Init();
}

void ParticleGenerator::Update(GLfloat dt, GameObject& object, GLuint newParticles, glm::vec2 offset)
{
	//添加新的粒子
	for (GLuint i = 0;i < newParticles;i++) {
		//找到第一个生命结束消亡的粒子
		int	unusedParticle = FirstUnusedParticle();
		RespawnParticle(Particles[unusedParticle], object, offset);
	}
	//更新所有的粒子
	for (GLuint i = 0;i < amount;i++) {
		Particle& p = Particles[i];
		p.Life -= dt;
		if (p.Life > 0.0f) {
			p.Position -= p.Velocity * dt;
			p.Color.a -= dt * 2.5f;
		}
	}
}

void ParticleGenerator::Draw()
{
	//粒子叠加在一起时颜色也可以叠加
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	shader.Use();
	for (auto particle : Particles) {
		if (particle.Life > 0.0f) {
			shader.SetUniformVec2("offset", particle.Position);
			shader.SetUniformVec4("color", particle.Color);
			texture.Bind();
			GLCall(glBindVertexArray(VAO));
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
			GLCall(glBindVertexArray(0));
		}
	}
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleGenerator::Init()
{
	//设置粒子的顶点缓冲对象
	GLuint VBO;
	GLfloat particle_quad[] = {
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof particle_quad, particle_quad, GL_STATIC_DRAW));
	glEnableVertexAttribArray(0);
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof GL_FLOAT, 0));
	GLCall(glBindVertexArray(0));

	//创建amount个粒子
	for (GLuint i = 0;i < amount;i++) {
		Particles.push_back(Particle());
	}
}

//最后一个使用的粒子索引
GLuint lastUsedParticle = 0;
GLuint ParticleGenerator::FirstUnusedParticle()
{
	for (GLuint i = lastUsedParticle;i < amount;i++) {
		if (Particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//如果没找到就从0开始到last再找一遍
	for (GLuint i = 0;i < lastUsedParticle;i++) {
		if (Particles[i].Life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}
	//如果没有粒子生命周期结束就返回索引0，频繁出现这种情况应该适当调短生命周期
	lastUsedParticle = 0;
	std::cout<<"All particles survive."<<std::endl;
	return 0;
}

void ParticleGenerator::RespawnParticle(Particle& particle, GameObject& object, glm::vec2 offset)
{
	//得到一个在-5~5之前的值
	GLfloat random = (rand() % 100 - 50) / 10.0f;
	GLfloat rColor = 0.5 + rand() % 100 / 100.0f;
	particle.Position = object.Position + random + offset;
	particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
	particle.Life = 1.0f;
	particle.Velocity = object.Velocity * 0.1f;
}
