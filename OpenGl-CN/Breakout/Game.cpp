#include "Game.h"
#include "ResourceManager.h"
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
	delete Player;
	delete Ball;
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
	//�������ײ�ײ��߽�����
	if (Ball->Position.y >= Height) {
		ResetLevel();
		ResetPlayer();
	}
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
	//����ש�����ײ
	for (GameObject& box : levels[level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) {//�����ײ���Ļ�
				//���ש�鲻��ʵ�ľ�����
				if (!box.IsSolid) {
					box.Destroyed = GL_TRUE;
				}
				//��ײ����
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				//ˮƽ��ײ
				if (dir == LEFT || dir == RIGHT) {
					//��תˮƽ�ٶ�
					Ball->Velocity.x = -Ball->Velocity.x;
					//�ض�λ����������ש����
					GLfloat penetration = Ball->Radius - glm::length(diff_vector);
					if (dir == LEFT)
						Ball->Position.x += penetration;
					else
						Ball->Position.x -= penetration;
				}
				//��ֱ��ײ
				else {
					Ball->Velocity.y = -Ball->Velocity.y;
					GLfloat penetration = Ball->Radius - glm::length(diff_vector);
					if (dir == UP)
						Ball->Position.y -= penetration;
					else
						Ball->Position.y += penetration;
				}
			}
		}
	}
	//�����ɫ�������ײ
	Collision collision = CheckCollision(*Ball, *Player);
	//��û�й̶����ҷ�����ײ
	if (!Ball->Stuck && std::get<0>(collision)) {
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		//���Բ�ľ൲�����ĵ�ˮƽ����
		GLfloat distence = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distence / (Player->Size.x / 2);

		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = BALL_VELOCITY.x * percentage * strength;
		//��Ϊ����ײ����֮��y����ٶȱ�Ȼ�����ϵ�
		Ball->Velocity.y = -std::abs(Ball->Velocity.y);
		//��֤����ٶ�ʸ������
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}

GLboolean Game::CheckCollision(GameObject& one, GameObject& two)
{
	// x�᷽����ײ
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x || two.Position.x + two.Size.x >= one.Position.x;
	// y�᷽����ײ
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y || two.Position.y + two.Size.y >= one.Position.y;
	// ֻ���������򶼷�����ײʱ������ײ
	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject& one, GameObject& two)
{
	//��ȡԲ��
	glm::vec2 center(one.Position + one.Radius);
	//��ȡAABB����Ϣ
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position + aabb_half_extents);
	//��ȡ�������ĵ�ʸ����
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	//���ļ���clamped����AABB�Ͼ���Բ����ĵ�
	glm::vec2 closest = aabb_center + clamped;
	difference = center - closest;
	//�������С�ڵ��ڰ뾶��˵��ײ����
	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0.0f, 0.0f));
}

Direction Game::VectorDirection(glm::vec2 target)
{
	/*
	��ʾ������ײ����������ĸ�����
	target ����ײ������(��Բ�ĳ���)�� ��������ɰ���ײ������ƽ�Ƶ����Ͻ�ԭ�㣬Ȼ�������Ͻ�ԭ���϶���4������Ȼ������������ĵ�˽���ж��ĸ���ײ���������ĸ����������
	max Խ��˵���н�ԽС��˵��Բ�ĵ������ķ�������Խ�����ĸ�����
	*/
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f), //��
		glm::vec2(0.0f,-1.0f),//��
		glm::vec2(1.0f,0.0f), //��
		glm::vec2(-1.0f,0.0f) //��
	};
	GLfloat max = 0.0f;
	GLuint best_match = -1;
	for (GLuint i = 0;i < 4;i++) {
		GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return Direction(best_match);
}

void Game::ResetLevel()
{
	if (level == 0) levels[0].Load("res/levels/one.txt", Width, Height * 0.5f);
	else if (level == 1) levels[1].Load("res/levels/two.txt", Width, Height * 0.5f);
	else if (level == 2) levels[2].Load("res/levels/three.txt", Width, Height * 0.5f);
	else if (level == 3) levels[3].Load("res/levels/four.txt", Width, Height * 0.5f);
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Ball->Reset(glm::vec2(Player->Position + glm::vec2(PLAYER_SIZE.x / 2 - Ball->Radius, -Ball->Radius * 2)),BALL_VELOCITY);
}
