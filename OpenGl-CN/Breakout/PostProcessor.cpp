#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height) :
	PostProcessorShader(shader), Width(width), Height(height), Texture(), Confuse(GL_FALSE), Shake(GL_FALSE), Chaos(GL_FALSE)
{
	GLCall(glGenRenderbuffers(1, &RBO));
	GLCall(glGenFramebuffers(1, &MSFBO));
	GLCall(glGenFramebuffers(1, &FBO));

	//使用多采样颜色缓冲区初始化渲染缓冲区存储
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, MSFBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
	GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, Width, Height));//为渲染缓冲对象分配空间，多重采样数量为8
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO));//将多采样渲染缓冲对象附加到帧缓冲区
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Failed to init MSFBO" << std::endl;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	Texture.Generate(Width, Height, NULL);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.GetID(), 0));//将纹理作为颜色附件添加到帧缓冲区
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Failed to init FBO" << std::endl;
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	//初始化渲染数据
	InitRenderData();
	PostProcessorShader.Use();
	PostProcessorShader.SetUniformInt("scene", 0);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset  },  // ↖        
		{  0.0f,    offset  },  // ↑
		{  offset,  offset  },  // ↗
		{ -offset,  0.0f    },  // ←
		{  0.0f,    0.0f    },  // ·
		{  offset,  0.0f    },  // →
		{ -offset, -offset  },  // ↙
		{  0.0f,   -offset  },  // ↓
		{  offset, -offset  }   // ↘
	};
	GLCall(glUniform2fv(glGetUniformLocation(PostProcessorShader.GetProgramID(),"offsets"), 9, (GLfloat*)offsets));
	//锐化
	GLint edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	GLCall(glUniform1iv(glGetUniformLocation(PostProcessorShader.GetProgramID(), "edge_kernel"), 9, (GLint*)edge_kernel));
	//模糊
	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	GLCall(glUniform1fv(glGetUniformLocation(PostProcessorShader.GetProgramID(), "blur_kernel"), 9, (GLfloat*)blur_kernel));
}

void PostProcessor::BeginRender()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, MSFBO));
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void PostProcessor::EndRender()
{
	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO));
	//一个多重采样的图像包含比普通图像更多的信息，我们所要做的是缩小或者还原(Resolve)图像。
	//将一个帧缓冲中的某个区域复制到另一个帧缓冲中，并且将多重采样缓冲还原。
	GLCall(glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void PostProcessor::Render(GLfloat time)
{
	PostProcessorShader.Use();
	PostProcessorShader.SetUniformFloat("time", time);
	PostProcessorShader.SetUniformInt("chaos", Chaos);
	PostProcessorShader.SetUniformInt("confuse", Confuse);
	PostProcessorShader.SetUniformInt("shake", Shake);

	GLCall(glActiveTexture(GL_TEXTURE0));
	Texture.Bind();
	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	GLCall(glBindVertexArray(0));
}

void PostProcessor::InitRenderData()
{
	GLuint VBO;
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};
	//生成
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	//绑定
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	//设置数据
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof GL_FLOAT, 0));
	//解绑
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}
