#pragma once

#include "Renderer.h"

#include <string>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
private:
	unsigned int Program;//����ID
public:
	//��ȡԴ�벢������ɫ��
	Shader(const std::string& filepath);
	~Shader();
	//ʹ�ó���
	void Use();
	void Unuse();
	unsigned int GetProgramID() { return Program; };
private:
	//��·����ָ���*.shaderԴ���еĶ����ɫ�����
	ShaderProgramSource ParseShader(const std::string& filepath);
	//����shaderԴ��
	GLuint CompileShader(GLuint type, const std::string& source);
	//����shader����
	GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};