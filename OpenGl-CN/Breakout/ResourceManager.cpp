#include "ResourceManager.h"
#include "../res/stb_image/stb_image.h"

#include <iostream>

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;

Shader ResourceManager::LoadShader(const GLchar* filepath, std::string name)
{
    Shaders[name] = Shader(filepath);
    return Shaders[name];
}

Shader ResourceManager::GetShader(std::string name)
{
    if (Shaders.find(name) == Shaders.end()) {
        std::cout << "failed to find : " << name << std::endl;
        ASSERT(0);
    }
    return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar* filepath, std::string name)
{
    Textures[name] = LoadTexture(filepath);
    return Textures[name];
}

Texture2D ResourceManager::GetTexture(std::string name)
{
    if (Textures.find(name) == Textures.end()) {
        std::cout << "failed to find : " << name << std::endl;
        ASSERT(0);
    }
    return Textures[name];
}

void ResourceManager::Clear()
{
    for (auto i : Shaders)
        glDeleteProgram(i.second.GetProgramID());
    for (auto i : Textures)
        glDeleteTextures(1, &i.second.GetID());
}

Texture2D ResourceManager::LoadTexture(const GLchar* filepath)
{
    Texture2D texture;
    int width, height, numberComponents;
    unsigned char* data = stbi_load(filepath, &width, &height, &numberComponents, 0);
    if (data) {
        texture.SetFormat(numberComponents);
        texture.Generate(width, height, data);
    }
    else {
        std::cout << "Texture failed to load at path : " << filepath << std::endl;
    }
    stbi_image_free(data);

    return texture;
}
