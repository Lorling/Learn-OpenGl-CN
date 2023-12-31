#pragma once

#include <irrKlang.h>

#include "Renderer.h"
#include "Shader.h"
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "PowerUp.h"
#include "TextRenderer.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN,
	GAME_FAIL,
};

enum Direction {
	UP,
	DOWN,
	RIGHT,
	LEFT
};

typedef std::tuple<GLboolean, Direction, glm::vec2> Collision;

class Game {
public:
	GameState State;
	GLboolean Keys[1024];
	//村粗已经被处理过的按键
	GLboolean KeysProcessed[1024];
	GLfloat Width, Height;
	std::vector<GameLevel> levels;
	std::vector<PowerUp> PowerUps;
	std::vector<BallObject> Balls;
	std::vector<ParticleGenerator> Particles;
	GLuint level;
	GLuint Life = 3;
	GLuint FailText;

	Game(GLfloat width, GLfloat height);
	~Game();
	//初始化游戏状态
	void Init();
	//游戏循环
	void ProccessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();

	//碰撞处理
	void DoCollisions(BallObject& ball);

	//碰撞检测
	GLboolean CheckCollision(GameObject& one, GameObject& two);
	Collision CheckCollision(BallObject& one, GameObject& two);

	//获得碰撞发生在相对于球的哪个方向
	Direction VectorDirection(glm::vec2 target);

	//重新设置角色和关卡
	void ResetLevel();
	void ResetPlayer();

	//生成、更新和激活道具
	void SpawnPowerUps(GameObject& Object);
	void UpdatePowerUps(GLfloat dt);
	void ActivatePowerUp(PowerUp& powerUp);
	GLboolean IsOtherPowerActive(std::vector<PowerUp>& powerUps, PROPTYPE type);
};