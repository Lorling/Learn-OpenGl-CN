#include "Game.h"
#include "ResourceManager.h"
#include <iostream>
#include <GLFW/glfw3.h>

//初始化角色挡板大小
const glm::vec2 PLAYER_SIZE(100, 20);
//初始化角色挡板速度
const GLfloat PLAYER_VELOCITY(500.0f);
//初始化球的速度
const glm::vec2 BALL_VELOCITY(100.0f, -350.f);
//球的半径
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

	//加载纹理
	ResourceManager::LoadTexture("res/textures/background.jpg", "background");
	ResourceManager::LoadTexture("res/textures/block.png", "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", "paddle");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", "ball");
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
	//初始化角色挡板
	glm::vec2 playerPos = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));
	//初始化球
	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -2 * BALL_RADIUS);//球左上角的位置
	Ball = new BallObject(ballPos, BALL_RADIUS, BALL_VELOCITY, ResourceManager::GetTexture("ball"));

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
}

void Game::ProccessInput(GLfloat dt)
{
	if (State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//移动挡板，并且让球跟着挡板移动
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
		//如果输入空格则释放球
		if (Keys[GLFW_KEY_SPACE]) {
			Ball->Stuck = GL_FALSE;
		}
	}
}

void Game::Update(GLfloat dt)
{
	//更新球的位置
	Ball->Move(dt, Width);
	//检测碰撞
	DoCollisions();
	//检测球碰撞底部边界死亡
	if (Ball->Position.y >= Height) {
		ResetLevel();
		ResetPlayer();
	}
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
		//绘制球
		Ball->Draw(*Renderer);
	}
}

void Game::DoCollisions()
{
	//球与砖块的碰撞
	for (GameObject& box : levels[level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(*Ball, box);
			if (std::get<0>(collision)) {//如果碰撞到的话
				//如果砖块不是实心就销毁
				if (!box.IsSolid) {
					box.Destroyed = GL_TRUE;
				}
				//碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				//水平碰撞
				if (dir == LEFT || dir == RIGHT) {
					//反转水平速度
					Ball->Velocity.x = -Ball->Velocity.x;
					//重定位，避免球挤在砖块内
					GLfloat penetration = Ball->Radius - glm::length(diff_vector);
					if (dir == LEFT)
						Ball->Position.x += penetration;
					else
						Ball->Position.x -= penetration;
				}
				//垂直碰撞
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
	//球与角色挡板的碰撞
	Collision collision = CheckCollision(*Ball, *Player);
	//球没有固定并且发生碰撞
	if (!Ball->Stuck && std::get<0>(collision)) {
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		//球的圆心距挡板中心的水平距离
		GLfloat distence = (Ball->Position.x + Ball->Radius) - centerBoard;
		GLfloat percentage = distence / (Player->Size.x / 2);

		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = Ball->Velocity;
		Ball->Velocity.x = BALL_VELOCITY.x * percentage * strength;
		//因为球碰撞挡板之后y轴的速度必然是向上的
		Ball->Velocity.y = -std::abs(Ball->Velocity.y);
		//保证球的速度矢量不变
		Ball->Velocity = glm::normalize(Ball->Velocity) * glm::length(oldVelocity);
	}
}

GLboolean Game::CheckCollision(GameObject& one, GameObject& two)
{
	// x轴方向碰撞
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x || two.Position.x + two.Size.x >= one.Position.x;
	// y轴方向碰撞
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y || two.Position.y + two.Size.y >= one.Position.y;
	// 只有两个轴向都发生碰撞时才算碰撞
	return collisionX && collisionY;
}

Collision Game::CheckCollision(BallObject& one, GameObject& two)
{
	//获取圆心
	glm::vec2 center(one.Position + one.Radius);
	//获取AABB的信息
	glm::vec2 aabb_half_extents(two.Size.x / 2, two.Size.y / 2);
	glm::vec2 aabb_center(two.Position + aabb_half_extents);
	//获取两个中心的矢量差
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
	//中心加上clamped就是AABB上距离圆最近的点
	glm::vec2 closest = aabb_center + clamped;
	difference = center - closest;
	//如果距离小于等于半径则说明撞到了
	if (glm::length(difference) <= one.Radius)
		return std::make_tuple(GL_TRUE, VectorDirection(difference), difference);
	else
		return std::make_tuple(GL_FALSE, UP, glm::vec2(0.0f, 0.0f));
}

Direction Game::VectorDirection(glm::vec2 target)
{
	/*
	表示的是碰撞发生在球的哪个方向
	target 是碰撞点向量(从圆心出发)， 可以想象成把碰撞点向量平移到左上角原点，然后在左上角原点上定义4个方向，然后根据下面计算的点乘结果判断哪个碰撞点向量离哪个方向更近。
	max 越大说明夹角越小，说明圆心到最近点的方向向量越靠近哪个方向。
	*/
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f), //上
		glm::vec2(0.0f,-1.0f),//下
		glm::vec2(1.0f,0.0f), //右
		glm::vec2(-1.0f,0.0f) //左
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
