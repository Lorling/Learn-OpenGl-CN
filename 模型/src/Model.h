#pragma once

#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model
{
private:
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;//所有加载过的纹理,来避免同一个纹理多次加载

	void loadModel(std::string filepath);
	void processNode(aiNode* node,const aiScene* scene);//处理场景中节点
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);//处理节点中网格
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);//从材质中获取纹理
	unsigned int TextureFromFile(const char* filepath, const std::string& directory);
public:
	Model(std::string filepath);
	void Draw(Shader shader);
};

