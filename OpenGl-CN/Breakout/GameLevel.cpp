#include "GameLevel.h"

#include "ResourceManager.h"

#include <sstream>
#include <fstream>
#include <iostream>

GameLevel::GameLevel()
{
}

void GameLevel::Load(const GLchar* filepath, GLuint levelWidth, GLuint levelHeight)
{
	//清理过期数据
	Bricks.clear();

	GLuint tileCode;
	GameLevel level;
	std::string line;
	std::ifstream fstream(filepath);
	std::vector<std::vector<GLuint>> tileData;
	if (fstream) {
		while (std::getline(fstream, line)) {
			std::istringstream sstream(line);
			std::vector<GLuint> row;
			while (sstream >> tileCode)//读取被空格分割的每个数字
				row.push_back(tileCode);
			tileData.push_back(row);
		}
		if (tileData.size() > 0)
			Init(tileData, levelWidth, levelHeight);
	}
}

void GameLevel::Draw(SpriteRenderer& renderer)
{
	for (auto& i : Bricks)
		if (!i.Destroyed) 
			i.Draw(renderer);
}

GLboolean GameLevel::IsCompleted()
{
	//如果有可以被摧毁的没有被摧毁
	for (auto& i : Bricks)
		if (!i.IsSolid && !i.Destroyed)
			return GL_FALSE;
	return GL_TRUE;
}

void GameLevel::Init(std::vector<std::vector<GLuint>> tileData, GLuint levelWidth, GLuint levelHeight)
{
	//计算每个维度的大小
	GLuint height = tileData.size();
	GLuint width = tileData[0].size();
	GLfloat unit_width = levelWidth / (GLfloat)width;
	GLfloat unit_height = levelHeight / (GLfloat)height;

	for (GLuint i = 0;i < height;i++) {
		for (GLuint j = 0;j < width;j++) {
			glm::vec3 color = glm::vec3(0.8f, 0.8f, 0.7f);
			glm::vec2 pos(unit_width * j, unit_height * i);
			glm::vec2 size(unit_width, unit_height);
			GameObject obj(pos, size, ResourceManager::GetTexture("block_solid"), color);
			//检测砖块类型
			if (tileData[i][j] == 1) {
				//不可被破坏
				obj.IsSolid = GL_TRUE;
				obj.SetSprite(ResourceManager::GetTexture("block_solid"));
			}
			else if (tileData[i][j] > 1) {
				//可以被破坏，不同颜色的砖块
				color = glm::vec3(1.0f);//默认白色
				if (tileData[i][j] == 2)
					color = glm::vec3(0.2f, 0.6f, 1.0f);
				else if (tileData[i][j] == 3)
					color = glm::vec3(0.0f, 0.7f, 0.0f);
				else if (tileData[i][j] == 4)
					color = glm::vec3(0.8f, 0.8f, 0.4f);
				else if (tileData[i][j] == 5)
					color = glm::vec3(1.0f, 0.5f, 0.0f);
				obj.SetColor(color);
				obj.SetSprite(ResourceManager::GetTexture("block"));
			}
			Bricks.push_back(obj);
		}
	}
}
