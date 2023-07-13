#include "Game.h"
#include "ResourceManager.h"
#include <iostream>
#include <GLFW/glfw3.h>

//初始化角色挡板大小
const glm::vec2 PLAYER_SIZE(100, 20);
//初始化角色挡板速度
const GLfloat PLAYER_VELOCITY(500.0f);

SpriteRenderer* Renderer;
GameObject* Player;

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

	//加载纹理
	ResourceManager::LoadTexture("res/textures/background.jpg", "background");
	ResourceManager::LoadTexture("res/textures/block.png", "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", "paddle");
	//加载关卡
	GameLevel one, two, three, four;
	one.Load("res/levels/one.txt", Width, Height * 0.5f);
	two.Load("res/levels/one.txt", Width, Height * 0.5f);
	three.Load("res/levels/one.txt", Width, Height * 0.5f);
	four.Load("res/levels/one.txt", Width, Height * 0.5f);
	levels.push_back(one);
	levels.push_back(two);
	levels.push_back(three);
	levels.push_back(four);
	level = 0;
	//
	glm::vec2 playerPos = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::ProccessInput(GLfloat dt)
{
	if (State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//移动挡板
		if (Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
			}
			if (Player->Position.x < 0) Player->Position.x = 0;
		}
		if (Keys[GLFW_KEY_D]) {
			if(Player->Position.x <= Width - PLAYER_SIZE.x)
				Player->Position.x += velocity;
			if (Player->Position.x > Width - PLAYER_SIZE.x) Player->Position.x = Width - PLAYER_SIZE.x;
		}
	}
}

void Game::Update(GLfloat dt)
{
}

void Game::Render()
{
	if (State == GAME_ACTIVE) {
		//绘制背景
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);
		//绘制关卡
		levels[level].Draw(*Renderer);
		//绘制角色挡板
		Player->Draw(*Renderer);
	}
}
