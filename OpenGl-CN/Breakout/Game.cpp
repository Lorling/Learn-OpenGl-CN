#include "Game.h"
#include "ResourceManager.h"
#include "BallObject.h"
#include <iostream>
#include <GLFW/glfw3.h>

//��ʼ����ɫ�����С
const glm::vec2 PLAYER_SIZE(100, 20);
//��ʼ����ɫ�����ٶ�
const GLfloat PLAYER_VELOCITY(500.0f);
//��ʼ������ٶ�
const glm::vec2 BALL_VELOCITY(100.0f, -350.f);
//��İ뾶
const GLfloat BALL_RADIUS = 12.5f;

SpriteRenderer* Renderer;
GameObject* Player;
BallObject* Ball;

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

	//��������
	ResourceManager::LoadTexture("res/textures/background.jpg", "background");
	ResourceManager::LoadTexture("res/textures/block.png", "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", "paddle");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", "ball");
	//���عؿ�
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
	//��ʼ����ɫ����
	glm::vec2 playerPos = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	//��ʼ����
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -2 * BALL_RADIUS);//�����Ͻǵ�λ��
	Ball = new BallObject(ballPos, BALL_RADIUS, BALL_VELOCITY, ResourceManager::GetTexture("ball"));

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::ProccessInput(GLfloat dt)
{
	if (State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//�ƶ����壬����������ŵ����ƶ�
		if (Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
				if (Ball->Stuck)
					Ball->Position.x -= velocity;
			}
			if (Player->Position.x < 0) Player->Position.x = 0;
		}
		if (Keys[GLFW_KEY_D]) {
			if (Player->Position.x <= Width - PLAYER_SIZE.x) {
				Player->Position.x += velocity;
				if (Ball->Stuck)
					Ball->Position.x += velocity;
			}
			if (Player->Position.x > Width - PLAYER_SIZE.x) Player->Position.x = Width - PLAYER_SIZE.x;
		}
		//�������ո����ͷ���
		if (Keys[GLFW_KEY_SPACE]) {
			Ball->Stuck = GL_FALSE;
		}
	}
}

void Game::Update(GLfloat dt)
{
	//�������λ��
	Ball->Move(dt, Width);
	//�����ײ
	DoCollisions();
}

void Game::Render()
{
	if (State == GAME_ACTIVE) {
		//���Ʊ���
		Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);
		//���ƹؿ�
		levels[level].Draw(*Renderer);
		//���ƽ�ɫ����
		Player->Draw(*Renderer);
		//������
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions()
{
	for (GameObject& box : levels[level].Bricks) {
		if (!box.Destroyed) {
			if (CheckCollision(box, *Ball)) {
				if (!box.IsSolid) {
					box.Destroyed = GL_TRUE;
				}
			}
		}
	}
}

GLboolean Game::CheckCollision(GameObject& one, GameObject& two)
{
	//x����ײ �����������������ı��ǲ��Ǵ�����һ����������ı�
	//û���ж������������һ��������������
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	//y����ײ
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
	//ֻ�������ᶼ��ײ������ײ
	return collisionX & collisionY;
}
