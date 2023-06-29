#pragma once

#include <iostream>

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
};