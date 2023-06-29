#pragma once

#include "Renderer.h"

#include <string>

struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader {
private:
	unsigned int Program;//程序ID
public:
	//读取源码并构造着色器
	Shader(const std::string& filepath);
	~Shader();
	//使用程序
	void Use();
	void Unuse();
	unsigned int GetProgramID() { return Program; };
private:
	//将路径所指向的*.shader源码中的多个着色器拆分
	ShaderProgramSource ParseShader(const std::string& filepath);
	//编译shader源码
	GLuint CompileShader(GLuint type, const std::string& source);
	//创建shader程序
	GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
};