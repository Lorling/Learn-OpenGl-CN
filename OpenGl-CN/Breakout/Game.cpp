﻿#include "Game.h"
#include "ResourceManager.h"
#include <iostream>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H 

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
ParticleGenerator* Particle;
PostProcessor* Effects;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();
TextRenderer* Text;

GLfloat ShakeTime = 0.0f;

Game::Game(GLfloat width, GLfloat height) :
	State(GAME_MENU), Keys(), Width(width), Height(height)
{
}

Game::~Game()
{
	delete SoundEngine;
	delete Renderer;
	delete Player;
	delete Ball;
	delete Particle;
	delete Effects;
	delete Text;
}

void Game::Init()
{
	SoundEngine->play2D("res/audio/breakout.mp3", GL_TRUE);
	//加载着色器
	ResourceManager::LoadShader("Breakout/shaders/Sprite.shader", "sprite");
	ResourceManager::LoadShader("Breakout/shaders/Particle.shader", "particle");
	ResourceManager::LoadShader("Breakout/shaders/Scene.shader", "scene");

	//正交投影矩阵
	glm::mat4 projection = glm::ortho(0.0f, Width, Height, 0.0f, -1.0f, 1.0f);//左边界，右边界，下边届，上边界，最近距离，最远距离
	ResourceManager::GetShader("sprite").Use();
	ResourceManager::GetShader("sprite").SetUniformInt("image", 0);
	ResourceManager::GetShader("sprite").SetUniformMatrix4fv("projection", projection);
	ResourceManager::GetShader("particle").Use();
	ResourceManager::GetShader("particle").SetUniformInt("image", 0);
	ResourceManager::GetShader("particle").SetUniformMatrix4fv("projection", projection);

	//加载纹理
	ResourceManager::LoadTexture("res/textures/background.jpg", "background");
	ResourceManager::LoadTexture("res/textures/block.png", "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", "paddle");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", "ball");
	ResourceManager::LoadTexture("res/textures/particle.png", "particle");
	ResourceManager::LoadTexture("res/textures/powerup_chaos.png", "chaos");
	ResourceManager::LoadTexture("res/textures/powerup_confuse.png", "confuse");
	ResourceManager::LoadTexture("res/textures/powerup_increase.png", "increase");
	ResourceManager::LoadTexture("res/textures/powerup_passthrough.png", "passthrough");
	ResourceManager::LoadTexture("res/textures/powerup_sticky.png", "sticky");
	ResourceManager::LoadTexture("res/textures/powerup_speed.png", "speed");
	//加载关卡
	GameLevel one, two, three, four;
	one.Load("res/levels/one.txt", Width, Height * 0.5f);
	two.Load("res/levels/two.txt", Width, Height * 0.5f);
	three.Load("res/levels/three.txt", Width, Height * 0.5f);
	four.Load("res/levels/four.txt", Width, Height * 0.5f);
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
	Balls.push_back(*Ball);

	Particle = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
	for (GLuint i = 0;i < 100;i++)
		Particles.push_back(*Particle);

	Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	Effects = new PostProcessor(ResourceManager::GetShader("scene"), Width, Height);

	Text = new TextRenderer(Width, Height);
	Text->Load("res/meow.TTF", 24);
}

void Game::ProccessInput(GLfloat dt)
{
	//在菜单界面进行关卡选择
	if (State == GAME_MENU) {
		if (Keys[GLFW_KEY_ENTER])
			State = GAME_ACTIVE;
		if (Keys[GLFW_KEY_W] && !KeysProcessed[GLFW_KEY_W]) {
			level = (level + 1) % 4;
			KeysProcessed[GLFW_KEY_W] = GL_TRUE;
		}
		if (Keys[GLFW_KEY_S] && !KeysProcessed[GLFW_KEY_S]) {
			if (level > 0)
				level--;
			else level = 3;
			KeysProcessed[GLFW_KEY_S] = GL_TRUE;
		}
			
	}
	if (State == GAME_ACTIVE) {
		GLfloat velocity = PLAYER_VELOCITY * dt;
		//移动挡板，并且让球跟着挡板移动
		if (Keys[GLFW_KEY_A]) {
			if (Player->Position.x >= 0) {
				Player->Position.x -= velocity;
				for(auto &ball : Balls)
					if (ball.Stuck) 
						if (ball.Position.x >= 0)
							ball.Position.x -= velocity;
			}
			if (Player->Position.x < 0) Player->Position.x = 0;
		}
		if (Keys[GLFW_KEY_D]) {
			if (Player->Position.x <= Width - Player->Size.x) {
				Player->Position.x += velocity;
				for(auto& ball : Balls)
					if (ball.Stuck)
						if (ball.Position.x <= Width - ball.Radius * 2) 
							ball.Position.x += velocity;
			}
			if (Player->Position.x > Width - Player->Size.x) Player->Position.x = Width - Player->Size.x;
		}
		//如果输入空格则释放球
		if (Keys[GLFW_KEY_SPACE]) {
			for(auto& ball : Balls)
				ball.Stuck = GL_FALSE;
		}
	}
	if (State == GAME_WIN) {
		if (Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER]) {
			KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			State = GAME_ACTIVE;
			ResetLevel();
			ResetPlayer();
			level = (level + 1) % 4;
		}
	}
	if (State == GAME_FAIL) {
		if (Keys[GLFW_KEY_ENTER] && !KeysProcessed[GLFW_KEY_ENTER]) {
			KeysProcessed[GLFW_KEY_ENTER] = GL_TRUE;
			State = GAME_ACTIVE;
			ResetLevel();
			ResetPlayer();
		}
	}
}

void Game::Update(GLfloat dt)
{
	//判断游戏胜利
	if (State == GAME_ACTIVE && levels[level].IsCompleted()) {
		State = GAME_WIN;
	}
	//更新球的位置
	for (auto& ball : Balls) {
		ball.Move(dt, Width);
		//检测碰撞
		DoCollisions(ball);
	}
	//移除触碰边界死亡的球
	if (State == GAME_ACTIVE) {
		static GLuint hei = Height;
		Balls.erase(std::remove_if(Balls.begin(), Balls.end(),
			[](const BallObject& ball) { return ball.Position.y >= hei; }
		), Balls.end());
	}
	if (Balls.size() == 0 && State == GAME_ACTIVE) {
		State = GAME_FAIL;
	}
	//粒子更新，并且产生的粒子偏向球中心
	for(GLuint i = 0;i<Balls.size();i++)
		Particles[i].Update(dt, Balls[i], 2, glm::vec2(Balls[i].Radius / 2));
	//更新shaketime
	if (ShakeTime > 0.0f) {
		ShakeTime -= dt;
		if (ShakeTime <= 0.0f)
			Effects->Shake = GL_FALSE;
	}
	UpdatePowerUps(dt);
}

void Game::Render()
{
	Effects->BeginRender();
	//绘制背景
	Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(Width, Height), 0.0f);
	//绘制关卡
	levels[level].Draw(*Renderer);
	//绘制角色挡板
	Player->Draw(*Renderer);
	//绘制粒子
	for(GLuint i = 0;i<Balls.size();i++)
		Particles[i].Draw();
	//绘制球
	for(auto& ball : Balls)
		ball.Draw(*Renderer);
	//绘制道具
	for (auto powerUp : PowerUps)
		if (!powerUp.Destroyed)
			powerUp.Draw(*Renderer);
	//绘制文本
	wchar_t LifeText[20];
	swprintf_s(LifeText, L"生命：%d", Life);
	wchar_t LevelText[20];
	swprintf_s(LevelText, L"关卡%d", level + 1);
	Text->RenderText(LifeText, 5.0f, 5.0f, 1.0f);
	Text->RenderText(LevelText, Width - 80.0f, 5.0f, 1.0f);
	if (State == GAME_MENU) {
		Text->RenderText(L"按 ENTER 开始游戏", 300.0f, Height / 2, 1.0f);
		Text->RenderText(LevelText, 360.0f, Height / 2 + 20.0f, 1.0f);
		Text->RenderText(L"按 W 或 S 选择关卡", 290.0f, Height / 2 + 40.0f, 1.0f);
		Text->RenderText(L"按 ESC 退出游戏", 310.0f, Height / 2 + 60.0f, 1.0f);
	}
	if (State == GAME_WIN) {
		Text->RenderText(L"牛", 340.0f, Height / 2 - 20, 1.0f, 100,glm::vec3(0.0f,1.0f,0.0f));
		Text->RenderText(L"按 ENTER 重新游戏", 300.0f, Height / 2 + 20.0f, 1.0f, 24, glm::vec3(1.0f,1.0f,0.0f));
		Text->RenderText(L"按 ESC 退出游戏", 310.0f, Height / 2 + 40.0f, 1.0f, 24, glm::vec3(1.0f, 1.0f, 0.0f));
	}
	if (State == GAME_FAIL) {
		switch (FailText)
		{
		case 0:
			Text->RenderText(L"菜", 340.0f, Height / 2 - 20, 1.0f, 100, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 1:
			Text->RenderText(L"笨", 340.0f, Height / 2 - 20, 1.0f, 100, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 2:
			Text->RenderText(L"该", 340.0f, Height / 2 - 20, 1.0f, 100, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 3:
			Text->RenderText(L"猪", 340.0f, Height / 2 - 20, 1.0f, 100, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		case 4:
			Text->RenderText(L"寄", 340.0f, Height / 2 - 20, 1.0f, 100, glm::vec3(0.0f, 1.0f, 0.0f));
			break;
		default:
			break;
		}
		Text->RenderText(L"按 ENTER 重新游戏", 300.0f, Height / 2 + 20.0f, 1.0f, 24, glm::vec3(1.0f, 1.0f, 0.0f));
		Text->RenderText(L"按 ESC 退出游戏", 310.0f, Height / 2 + 40.0f, 1.0f, 24, glm::vec3(1.0f, 1.0f, 0.0f));
	}
	Effects->EndRender();
	Effects->Render(glfwGetTime());
}

void Game::DoCollisions(BallObject& ball)
{
	//球与砖块的碰撞
	for (GameObject& box : levels[level].Bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(ball, box);
			if (std::get<0>(collision)) {//如果碰撞到的话
				//如果砖块不是实心就销毁
				if (!box.IsSolid) {
					SoundEngine->play2D("res/audio/bleep.mp3", GL_FALSE);
					box.Destroyed = GL_TRUE;
					SpawnPowerUps(box);
				}
				else {
					SoundEngine->play2D("res/audio/solid.wav", GL_FALSE);
					//如果是实心球则触发shake特效
					ShakeTime = 0.05f;
					Effects->Shake = GL_TRUE;
				}
				//如果球可以穿过就不需要碰撞处理
				if (ball.PassThrough && !box.IsSolid) continue;
				//碰撞处理
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				//水平碰撞
				if (dir == LEFT || dir == RIGHT) {
					//反转水平速度
					ball.Velocity.x = -ball.Velocity.x;
					//重定位，避免球挤在砖块内
					GLfloat penetration = ball.Radius - glm::length(diff_vector);
					if (dir == LEFT)
						ball.Position.x += penetration;
					else
						ball.Position.x -= penetration;
				}
				//垂直碰撞
				else {
					ball.Velocity.y = -ball.Velocity.y;
					GLfloat penetration = ball.Radius - glm::length(diff_vector);
					if (dir == UP)
						ball.Position.y += penetration;
					else
						ball.Position.y -= penetration;
				}
			}
		}
	}
	//球与角色挡板的碰撞
	Collision collision = CheckCollision(ball, *Player);
	//球没有固定并且发生碰撞
	if (!ball.Stuck && std::get<0>(collision)) {
		//根据道具更新状态
		ball.Stuck = ball.Sticky;
		if (ball.Stuck) ball.Position.y = Height - Player->Size.y - ball.Radius * 2;
		if (!ball.Stuck) SoundEngine->play2D("res/audio/bleep.wav", GL_FALSE);
		GLfloat centerBoard = Player->Position.x + Player->Size.x / 2;
		//球的圆心距挡板中心的水平距离
		GLfloat distence = (ball.Position.x + ball.Radius) - centerBoard;
		GLfloat percentage = distence / (Player->Size.x / 2);

		GLfloat strength = 2.0f;
		glm::vec2 oldVelocity = ball.Velocity;
		ball.Velocity.x = BALL_VELOCITY.x * percentage * strength;
		//因为球碰撞挡板之后y轴的速度必然是向上的
		ball.Velocity.y = -std::abs(ball.Velocity.y);
		//保证球的速度矢量不变
		ball.Velocity = glm::normalize(ball.Velocity) * glm::length(oldVelocity);
	}
	//角色挡板与道具的碰撞
	for (auto& powerUp : PowerUps) {
		if (!powerUp.Destroyed) {
			//如果出界就销毁
			if (powerUp.Position.y >= Height)
				powerUp.Destroyed = GL_TRUE;
			//使用AABB碰撞检测
			if (CheckCollision(*Player, powerUp)) {
				SoundEngine->play2D("res/audio/powerup.wav", GL_FALSE);
				ActivatePowerUp(powerUp);
				powerUp.Destroyed = GL_TRUE;
				powerUp.Activated = GL_TRUE;
			}
		}
	}
}

GLboolean Game::CheckCollision(GameObject& one, GameObject& two)
{
	// x轴方向碰撞
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x && two.Position.x + two.Size.x >= one.Position.x;
	// y轴方向碰撞
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y && two.Position.y + two.Size.y >= one.Position.y;
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
	//如果距离小于半径则说明撞到了
	//如果加上等于的话，碰到实心球修正之后的位置还是在碰撞范围内
	if (glm::length(difference) < one.Radius)
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
	Life = 3;
	if (level == 0) levels[0].Load("res/levels/one.txt", Width, Height * 0.5f);
	else if (level == 1) levels[1].Load("res/levels/two.txt", Width, Height * 0.5f);
	else if (level == 2) levels[2].Load("res/levels/three.txt", Width, Height * 0.5f);
	else if (level == 3) levels[3].Load("res/levels/four.txt", Width, Height * 0.5f);
}

void Game::ResetPlayer()
{
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(Width / 2 - PLAYER_SIZE.x / 2, Height - PLAYER_SIZE.y);
	Player->Color = glm::vec3(1.0f);
	Balls.clear();
	Balls.push_back(*Ball);

	//清理道具
	PowerUps.clear();
	Effects->Confuse = GL_FALSE;
	Effects->Chaos = GL_FALSE;
}

void Game::SpawnPowerUps(GameObject& Object)
{
	GLuint chance = 50;
	GLuint random = rand() % chance;
	//生成有益道具
	switch (random) {
	case 0:
		PowerUps.push_back(PowerUp(SPEED, 0.0f, glm::vec3(0.5f, 0.5f, 1.0f), Object.Position, ResourceManager::GetTexture(("speed"))));
		break;
	case 1:
		PowerUps.push_back(PowerUp(STICKY, 20.0f, glm::vec3(1.0f, 0.5f, 1.0f), Object.Position, ResourceManager::GetTexture(("sticky"))));
		break;
	case 2:
		PowerUps.push_back(PowerUp(PASS_THROUGH, 10.0f, glm::vec3(0.5f, 1.0f, 0.5f), Object.Position, ResourceManager::GetTexture(("passthrough"))));
		break;
	case 3:
		PowerUps.push_back(PowerUp(PAD_SIZE_INCREASE, 0.0f, glm::vec3(1.0f, 0.6f, 0.4f), Object.Position, ResourceManager::GetTexture(("increase"))));
		break;
	case 4:
		PowerUps.push_back(PowerUp(SEPARATION, 0.0f, glm::vec3(0.0f, 1.0f, 1.0f), Object.Position, ResourceManager::GetTexture("confuse")));
		break;
	//生成无益道具
	case 5:
		PowerUps.push_back(PowerUp(CHAOS, 15.0f, glm::vec3(1.0f, 0.3f, 0.3f), Object.Position, ResourceManager::GetTexture("chaos")));
		break;
	case 6:
		PowerUps.push_back(PowerUp(CONFUSE, 15.0f, glm::vec3(0.9f, 0.25f, 0.25f), Object.Position, ResourceManager::GetTexture("confuse")));
		break;
	default:
		break;
	}
}

void Game::UpdatePowerUps(GLfloat dt)
{
	for (auto& powerUp : PowerUps) {
		powerUp.Position += powerUp.Velocity * dt;
		if (powerUp.Activated) {
			powerUp.Duration -= dt;
			//持续时间结束停用效果
			//如果有同类道具在运行就不需要停用
			if (powerUp.Duration <= 0.0f) {
				powerUp.Activated = GL_FALSE;
				switch (powerUp.Type)
				{
				case STICKY:
					if (!IsOtherPowerActive(PowerUps, STICKY)) {
						for(auto& ball : Balls)
							ball.Sticky = GL_FALSE;
						Player->Color = glm::vec3(1.0f);
					}
					break;
				case PASS_THROUGH:
					if (!IsOtherPowerActive(PowerUps, PASS_THROUGH)) {
						for (auto& ball : Balls) {
							ball.PassThrough = GL_FALSE;
							ball.Color = glm::vec3(1.0f);
						}
					}
					break;
				case CONFUSE:
					if (!IsOtherPowerActive(PowerUps, CONFUSE)) 
						Effects->Confuse = GL_FALSE;
					break;
				case CHAOS:
					if (!IsOtherPowerActive(PowerUps, CHAOS))
						Effects->Chaos = GL_FALSE;
				default:
					break;
				}
			}
		}
	}
	//移除已经被销毁且没有在活动的道具
	PowerUps.erase(std::remove_if(PowerUps.begin(), PowerUps.end(),
		[](const PowerUp& powerUp) { return powerUp.Destroyed && !powerUp.Activated; }
	), PowerUps.end());
}

void Game::ActivatePowerUp(PowerUp& powerUp)
{
	switch (powerUp.Type)
	{
	case SPEED:
		for(auto& ball : Balls)
			ball.Velocity *= 1.2f;
		break;
	case STICKY:
		for (auto& ball : Balls)
			ball.Sticky = GL_TRUE;
		Player->Color = glm::vec3(1.0f, 0.5, 1.0f);
		break;
	case PASS_THROUGH:
		for (auto& ball : Balls) {
			ball.PassThrough = GL_TRUE;
			ball.Color = glm::vec3(1.0f, 0.5f, 0.5f);
		}
		break;
	case PAD_SIZE_INCREASE:
		Player->Size.x += 50;
		break;
	case SEPARATION:
	{
		GLuint random = rand() % Balls.size();
		auto ball = Balls[random];
		GLfloat vx = rand();
		GLfloat vy = -std::abs(rand());
		Balls.push_back(BallObject(ball.Position, ball.Radius, glm::normalize(glm::vec2(vx, vy)) * glm::length(ball.Velocity), ResourceManager::GetTexture("ball"), ball.Stuck));
	}
		break;
	case CHAOS:
		//只有未激活时生效
		if (!Effects->Chaos)
			Effects->Chaos = GL_TRUE;
	case CONFUSE:
		if (!Effects->Confuse)
			Effects->Confuse = GL_TRUE;
	default:
		break;
	}
}

GLboolean Game::IsOtherPowerActive(std::vector<PowerUp>& powerUps, PROPTYPE type)
{
	for (const auto powerUp : powerUps) {
		if (powerUp.Type == type && powerUp.Activated)
			return GL_TRUE;
	}
	return GL_FALSE;
}
