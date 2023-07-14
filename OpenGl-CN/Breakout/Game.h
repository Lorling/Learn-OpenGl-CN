#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "BallObject.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GMAE_WIN
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
	GLfloat Width, Height;
	std::vector<GameLevel> levels;
	GLuint level;

	Game(GLfloat width, GLfloat height);
	~Game();
	//初始化游戏状态
	void Init();
	//游戏循环
	void ProccessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();

	void DoCollisions();

	GLboolean CheckCollision(GameObject& one, GameObject& two);
	Collision CheckCollision(BallObject& one, GameObject& two);

	Direction VectorDirection(glm::vec2 target);

	void ResetLevel();
	void ResetPlayer();
};