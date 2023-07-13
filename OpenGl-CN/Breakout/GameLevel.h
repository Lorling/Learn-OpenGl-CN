#pragma once

#include "GameObject.h"

#include <vector>

class GameLevel
{
public:
	std::vector<GameObject> Bricks;

	GameLevel();
	//���ļ��м��عؿ�
	void Load(const GLchar* filepath, GLuint levelWidth, GLuint levelHeight);
	//��Ⱦ�ؿ�
	void Draw(SpriteRenderer& renderer);
	//���һ���ؿ��Ƿ�ͨ��
	GLboolean IsCompleted();
private:
	//��ש�����ݳ�ʼ���ؿ�
	void Init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

