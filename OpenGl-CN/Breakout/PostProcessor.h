#pragma once

#include "Texture.h"
#include "Shader.h"

class PostProcessor
{
public:
	Shader PostProcessorShader;
	Texture2D Texture;
	GLuint Width, Height;
	//渲染选项
	GLboolean Chaos;
	GLboolean Shake;//屏幕抖动与模糊
	GLboolean Confuse;

	PostProcessor(Shader shader,GLuint width,GLuint height);

	void BeginRender();//这个函数开始对游戏场景的渲染
	void EndRender();

	void Render(GLfloat time);
private:
	GLuint VAO;
	GLuint RBO;
	GLuint MSFBO;//多采样
	GLuint FBO;//常规帧缓冲

	void InitRenderData();
};

