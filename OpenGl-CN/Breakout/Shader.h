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
	unsigned int Program;//����ID
public:
	//��ȡԴ�벢������ɫ��
	Shader() { }
	Shader(const std::string& filepath);
	~Shader();
	//ʹ�ó���
	void Use();
	void Unuse();
	unsigned int GetProgramID() { return Program; };
	void SetUniformVec3(const char* uniformName, float f1, float f2, float f3);
	void SetUniformVec3(const char* uniformName, glm::vec3 vector);
	void SetUniformMatrix4fv(const char* uniformName, glm::mat4 matrix);
	void SetUniformFloat(const char* uniformName, float f);
	void SetUniformInt(const char* uniformName, int t);
private:
	//��·����ָ���*.shaderԴ���еĶ����ɫ�����
	void ParseShader(const std::string& filepath);
	//����shaderԴ��
	GLuint CompileShader(GLuint type, const std::string& source);
	//����shader����
	void CreateShader(ShaderType& type, const std::string& sourceShader);
};