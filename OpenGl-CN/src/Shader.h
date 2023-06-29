#pragma once

#include <iostream>

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
};