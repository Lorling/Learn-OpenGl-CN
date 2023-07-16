#pragma once

#include "Shader.h"
#include "Texture.h"

#include <map>

struct Character {
	GLuint TextureID;
	//ivecλint��
	glm::ivec2 Size;
	glm::ivec2 Bearing;//�ӻ��ߵ����Ͷ�����ƫ��
	GLuint Advance;//����һ���ַ���ˮƽƫ��
};

class TextRenderer
{
public:
	std::map<wchar_t, Character> Characters;

	Shader TextShader;
	
	TextRenderer(GLuint width, GLuint height);
	//����ttf�ļ�
	void Load(std::string filepath, GLuint fontSize, wchar_t c = ' ', GLboolean f = GL_FALSE);

	void RenderText(const wchar_t* text, GLfloat x, GLfloat y, GLfloat scale, GLuint fontSize = 24, glm::vec3 color = glm::vec3(1.0f));
private:
	GLuint VAO;
	GLuint VBO;
	std::string Filepath;
	GLuint FontSize;
};

