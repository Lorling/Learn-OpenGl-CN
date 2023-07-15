#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "SpriteRenderer.h"
#include "GameLevel.h"
#include "BallObject.h"
#include "ParticleGenerator.h"
#include "PostProcessor.h"
#include "PowerUp.h"

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
	std::vector<PowerUp> PowerUps;
	GLuint level;

	Game(GLfloat width, GLfloat height);
	~Game();
	//��ʼ����Ϸ״̬
	void Init();
	//��Ϸѭ��
	void ProccessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();

	//��ײ����
	void DoCollisions();

	//��ײ���
	GLboolean CheckCollision(GameObject& one, GameObject& two);
	Collision CheckCollision(BallObject& one, GameObject& two);

	//�����ײ���������������ĸ�����
	Direction VectorDirection(glm::vec2 target);

	//�������ý�ɫ�ȹؿ�
	void ResetLevel();
	void ResetPlayer();

	//���ɡ����ºͼ������
	void SpawnPowerUps(GameObject& Object);
	void UpdatePowerUps(GLfloat dt);
	void ActivatePowerUp(PowerUp& powerUp);
	GLboolean IsOtherPowerActive(std::vector<PowerUp>& powerUps, PROPTYPE type);
};