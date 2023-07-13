#pragma once

#include "GameObject.h"

#include <vector>

class GameLevel
{
public:
	std::vector<GameObject> Bricks;

	GameLevel();
	//从文件中加载关卡
	void Load(const GLchar* filepath, GLuint levelWidth, GLuint levelHeight);
	//渲染关卡
	void Draw(SpriteRenderer& renderer);
	//检测一个关卡是否通关
	GLboolean IsCompleted();
private:
	//由砖块数据初始化关卡
	void Init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight);
};

