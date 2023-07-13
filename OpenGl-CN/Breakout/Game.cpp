#include "Game.h"
#include "ResourceManager.h"
#include <iostream>

SpriteRenderer* Renderer;

Game::Game(GLfloat width, GLfloat height) :
	State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
	delete Renderer;
}

void Game::Init()
{
	ResourceManager::LoadShader("Breakout/shaders/Sprite.shader", "sprite");

	glm::mat4 projection = glm::ortho(0.0f, Width, Height, 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetUniformInt("image", 0);
	ResourceManager::GetShader("sprite").SetUniformMatrix4fv("projection", projection);

	ResourceManager::LoadTexture("res/textures/awesomeface.png", "face");

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::ProccessInput(GLfloat dt)
{
}

void Game::Update(GLfloat dt)
{
}

void Game::Render()
{
	Renderer->DrawSprite(ResourceManager::GetTexture("face"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}
