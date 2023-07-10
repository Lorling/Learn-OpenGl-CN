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
	std::vector<Texture> textures_loaded;//���м��ع�������,������ͬһ�������μ���

	void loadModel(std::string filepath);
	void processNode(aiNode* node,const aiScene* scene);//�������нڵ�
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);//����ڵ�������
	std::vector<Texture> loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName);//�Ӳ����л�ȡ����
	unsigned int TextureFromFile(const char* filepath, const std::string& directory);
public:
	Model(std::string filepath);
	void Draw(Shader shader);
};

