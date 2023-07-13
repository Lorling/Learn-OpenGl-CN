#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"

#include <string>

enum class ShaderType {
	NONE = -1, VERTEX = 0, FRAGMENT = 1
};

class Shader {
private:
	unsigned int Program;//程序ID
public:
	//读取源码并构造着色器
	Shader() { }
	Shader(const std::string& filepath);
	~Shader();
	//使用程序
	void Use();
	void Unuse();
	unsigned int GetProgramID() { return Program; };
	void SetUniformVec3(const char* uniformName, float f1, float f2, float f3);
	void SetUniformVec3(const char* uniformName, glm::vec3 vector);
	void SetUniformMatrix4fv(const char* uniformName, glm::mat4 matrix);
	void SetUniformFloat(const char* uniformName, float f);
	void SetUniformInt(const char* uniformName, int t);
private:
	//将路径所指向的*.shader源码中的多个着色器拆分
	void ParseShader(const std::string& filepath);
	//编译shader源码
	GLuint CompileShader(GLuint type, const std::string& source);
	//创建shader程序
	void CreateShader(ShaderType& type, const std::string& sourceShader);
};