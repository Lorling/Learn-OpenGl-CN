#include "PostProcessor.h"

#include <iostream>

PostProcessor::PostProcessor(Shader shader, GLuint width, GLuint height) :
	PostProcessorShader(shader), Width(width), Height(height), Texture(), Confuse(GL_FALSE), Shake(GL_FALSE), Chaos(GL_FALSE)
{
	GLCall(glGenRenderbuffers(1, &RBO));
	GLCall(glGenFramebuffers(1, &MSFBO));
	GLCall(glGenFramebuffers(1, &FBO));

	//ʹ�ö������ɫ��������ʼ����Ⱦ�������洢
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, MSFBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, RBO));
	GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, Width, Height));//Ϊ��Ⱦ����������ռ䣬���ز�������Ϊ8
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO));//���������Ⱦ������󸽼ӵ�֡������
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Failed to init MSFBO" << std::endl;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
	Texture.Generate(Width, Height, NULL);
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.GetID(), 0));//��������Ϊ��ɫ������ӵ�֡������
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Failed to init FBO" << std::endl;
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	//��ʼ����Ⱦ����
	InitRenderData();
	PostProcessorShader.Use();
	PostProcessorShader.SetUniformInt("scene", 0);
	GLfloat offset = 1.0f / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset  },  // �I        
		{  0.0f,    offset  },  // ��
		{  offset,  offset  },  // �J
		{ -offset,  0.0f    },  // ��
		{  0.0f,    0.0f    },  // ��
		{  offset,  0.0f    },  // ��
		{ -offset, -offset  },  // �L
		{  0.0f,   -offset  },  // ��
		{  offset, -offset  }   // �K
	};
	GLCall(glUniform2fv(glGetUniformLocation(PostProcessorShader.GetProgramID(),"offsets"), 9, (GLfloat*)offsets));
	//��
	GLint edge_kernel[9] = {
		-1, -1, -1,
		-1,  8, -1,
		-1, -1, -1
	};
	GLCall(glUniform1iv(glGetUniformLocation(PostProcessorShader.GetProgramID(), "edge_kernel"), 9, (GLint*)edge_kernel));
	//ģ��
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
	//һ�����ز�����ͼ���������ͨͼ��������Ϣ��������Ҫ��������С���߻�ԭ(Resolve)ͼ��
	//��һ��֡�����е�ĳ�������Ƶ���һ��֡�����У����ҽ����ز������廹ԭ��
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
	//����
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glGenBuffers(1, &VBO));
	//��
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	//��������
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof GL_FLOAT, 0));
	//���
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}
