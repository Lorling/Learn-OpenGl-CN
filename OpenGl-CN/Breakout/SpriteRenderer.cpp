#include "SpriteRenderer.h"

SpriteRenderer::SpriteRenderer(const Shader& shader)
{
    this->shader = shader;
    InitRendererData();
}

SpriteRenderer::~SpriteRenderer()
{
    GLCall(glDeleteVertexArrays(1, &this->quadVAO));
}

void SpriteRenderer::DrawSprite(const Texture2D& texture, glm::vec2 position,
    glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    this->shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    //旋转默认是围绕原点进行的，这里我们要将原点移到图形中心
    model = glm::translate(model, glm::vec3(0.5 * size.x, 0.5 * size.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5 * size.x, -0.5 * size.y, 0.0f));

    model = glm::scale(model, glm::vec3(size, 1.0f));

    shader.SetUniformMatrix4fv("model", model);
    shader.SetUniformVec3("spritecolor", color);

    GLCall(glActiveTexture(GL_TEXTURE0));
    texture.Bind();

    GLCall(glBindVertexArray(quadVAO));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    GLCall(glBindVertexArray(0));
}

void SpriteRenderer::InitRendererData()
{
	GLuint VBO;
    GLfloat vertices[] = {
        // 位置     // 纹理
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    GLCall(glGenVertexArrays(1, &this->quadVAO));
    GLCall(glGenBuffers(1, &VBO));

    GLCall(glBindVertexArray(this->quadVAO));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof GLfloat, 0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
}
