#pragma once

#include "Texture.h"
#include "Shader.h"

class PostProcessor
{
public:
	Shader PostProcessorShader;
	Texture2D Texture;
	GLuint Width, Height;
	//��Ⱦѡ��
	GLboolean Chaos;
	GLboolean Shake;//��Ļ������ģ��
	GLboolean Confuse;

	PostProcessor(Shader shader,GLuint width,GLuint height);

	void BeginRender();//���������ʼ����Ϸ��������Ⱦ
	void EndRender();

	void Render(GLfloat time);
private:
	GLuint VAO;
	GLuint RBO;
	GLuint MSFBO;//�����
	GLuint FBO;//����֡����

	void InitRenderData();
};

