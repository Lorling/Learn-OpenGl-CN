#pragma once

#include "Shader.h"
#include "Texture.h"

#include <map>

class ResourceManager
{
public:
	static std::map<std::string, Shader> Shaders;
	static std::map<std::string, Texture2D> Textures;
	static Shader LoadShader(const GLchar* filepath, std::string name);
	static Shader GetShader(std::string name);
	static Texture2D LoadTexture(const GLchar* filepath, std::string name);
	static Texture2D GetTexture(std::string name);
	static void Clear();
private:
	ResourceManager() { }
	static Texture2D LoadTexture(const GLchar* filepath);
};
