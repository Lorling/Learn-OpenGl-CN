#include "Model.h"

#include "../res/stb_image/stb_image.h"

Model::Model(std::string filepath)
{
	loadModel(filepath);
}

void Model::Draw(Shader shader)
{
	for (unsigned int i = 0;i < meshes.size();i++) {
		meshes[i].Draw(shader);
	}
}

void Model::loadModel(std::string filepath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filepath, aiProcess_Triangulate | aiProcess_FlipUVs);//Triangulate表示如果模型不是全部由三角形构成则将所有图元变换为三角形，FlipUVs表示翻转y轴的纹理坐标
	//aiProcess_GenNormals：如果模型不包含法向量的话，就为每个顶点创建法线。
	//aiProcess_SplitLargeMeshes：将比较大的网格分割成更小的子网格，如果你的渲染有最大顶点数限制，只能渲染较小的网格，那么它会非常有用。
	//aiProcess_OptimizeMeshes：和上个选项相反，它会将多个小网格拼接为一个大的网格，减少绘制调用从而进行优化。

	//检查场景和其根节点不为null，并且检查了它的一个标记(Flag)，来查看返回的数据是不是不完整的
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR:ASSIIMP:" << importer.GetErrorString() << std::endl;
		return ;
	}

	//文件路径的目录路径
	directory = filepath.substr(0, filepath.find_last_of('/'));
	
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//处理节点所有的网格
	for (unsigned int i = 0;i < node->mNumMeshes;i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh,scene));
	}
	//对子节点重复这一过程
	for (unsigned int i = 0;i < node->mNumChildren;i++) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//处理顶点属性
	for (unsigned int i = 0;i < mesh->mNumVertices;i++) {
		Vertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		if (mesh->HasNormals()) {
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->mTextureCoords[0]) {//网格是否由纹理坐标
			vertex.TexCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.TexCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else vertex.TexCoords = glm::vec2(0.0f);

		vertices.push_back(vertex);
	}
	//处理索引
	for (unsigned int i = 0;i < mesh->mNumFaces;i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0;j < face.mNumIndices;j++) {
			indices.push_back(face.mIndices[j]);
		}
	}
	//处理材质
	if (mesh->mMaterialIndex > 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}
	
	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0;i < material->GetTextureCount(type);i++) {
		aiString str;
		material->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0;j < textures_loaded.size();j++) {
			if (std::strcmp(textures_loaded[j].filepath.c_str(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (skip) continue;
		Texture texture;
		texture.id = TextureFromFile(str.C_Str(), directory);
		texture.type = typeName;
		texture.filepath = str.C_Str();

		textures.push_back(texture);
		textures_loaded.push_back(texture);
	}
	return textures;
}

unsigned int Model::TextureFromFile(const char* filepath, const std::string& directory)
{
	std::string filename = std::string(filepath);
	filename = directory + '/' + filename;

	unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));

	int width, height, numberComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &numberComponents, 0);
	if (data) {
		GLenum fromat;
		if (numberComponents == 1) fromat = GL_RED;
		else if (numberComponents == 3) fromat = GL_RGB;
		else if (numberComponents == 4) fromat = GL_RGBA;

		GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, fromat, width, height, 0, fromat, GL_UNSIGNED_BYTE, data));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else {
		std::cout << "Texture failed to load at :" << filename << std::endl;
		ASSERT(0);
	}
	stbi_image_free(data);

	return textureID;
}
