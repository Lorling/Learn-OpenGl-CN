#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices, std::vector<Texture> _textures) :
	vertices(_vertices), indices(_indices), textures(_textures)
{
	setupMesh();
}

void Mesh::Draw(Shader shader)
{
    unsigned int diffuseNumber = 1;
    unsigned int specularNumber = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // 在绑定之前激活相应的纹理单元
        // 获取纹理序号（diffuse_textureN 中的 N）
        std::string number;
        std::string name = textures[i].type;
        if (name == "texture_diffuse")
            number = std::to_string(diffuseNumber++);
        else if (name == "texture_specular")
            number = std::to_string(specularNumber++);

        shader.SetUniformInt((name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    // 绘制网格
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);//生成顶点数组对象
    glGenBuffers(1, &VBO);//生成顶点缓冲对象
    glGenBuffers(1, &EBO);//生成索引数组

    glBindVertexArray(VAO);//绑定顶点数组对象

    glBindBuffer(GL_ARRAY_BUFFER, VBO);//绑定顶点缓冲对象
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);//设置缓冲区数据

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);//绑定索引缓冲区
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);//设置索引缓冲区数据

    // 顶点位置
    glEnableVertexAttribArray(0);//激活顶点属性，索引0位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);//设置顶点属性,第一个参数为位置值，第二个为属性数量，第三个为参数类型，第四个为是否标准化，第五个为步长，第六个为起始位置
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
