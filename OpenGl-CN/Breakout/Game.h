#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "SpriteRenderer.h"

enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GMAE_WIN
};

class Game {
public:
	GameState State;
	GLboolean Keys[1024];
	GLfloat Width, Height;

	Game(GLfloat width, GLfloat height);
	~Game();
	//初始化游戏状态
	void Init();
	//游戏循环
	void ProccessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
};