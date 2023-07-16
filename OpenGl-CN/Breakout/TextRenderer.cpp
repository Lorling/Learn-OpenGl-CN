#include "TextRenderer.h"
#include "ResourceManager.h"

#include <iostream>
#include <ft2build.h>
#include FT_FREETYPE_H	

TextRenderer::TextRenderer(GLuint width, GLuint height)
{
	TextShader = ResourceManager::LoadShader("Breakout/shaders/Text.shader", "text");
	TextShader.Use();
	TextShader.SetUniformMatrix4fv("projection", glm::ortho(0.0f, (GLfloat)width, (GLfloat)height, 0.0f));
	TextShader.SetUniformInt("text", 0);
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glGenVertexArrays(1, &VAO));
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));

	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof GLfloat * 4 * 6, NULL, GL_STATIC_DRAW));//只分配空间
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof GL_FLOAT, 0));

	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void TextRenderer::Load(std::string filepath, GLuint fontSize, wchar_t c, GLboolean f)
{
	Filepath = filepath;
	FontSize = fontSize;
	//先清除之前的
	//Characters.clear();
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: init" << std::endl;
	FT_Face face;
	if (FT_New_Face(ft, filepath.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: load" << std::endl;
	//设置加载字型的大小
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	//采用 2 字节的 Unicode 编码
	FT_Select_Charmap(face, ft_encoding_unicode);
	//禁用字节对齐限制
	GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
	if (!f) {
		for (GLubyte c = 0;c < 128;c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				std::cout << "error load to " << c << std::endl;
				continue;
			}
			GLuint texture;
			GLCall(glGenTextures(1, &texture));
			GLCall(glBindTexture(GL_TEXTURE_2D, texture));
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			//设置环绕方式
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Character character = { texture,glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),face->glyph->advance.x };
			Characters.insert(std::pair<wchar_t, Character>(c, character));
		}
	}
	else {
		
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) 
			std::cout << "error load to " << c << std::endl;
		GLuint texture;
		GLCall(glGenTextures(1, &texture));
		GLCall(glBindTexture(GL_TEXTURE_2D, texture));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		//设置环绕方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		Character character = { texture,glm::ivec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),glm::ivec2(face->glyph->bitmap_left,face->glyph->bitmap_top),face->glyph->advance.x };
		Characters.insert(std::pair<wchar_t, Character>(c, character));
	}
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(const wchar_t* text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	TextShader.Use();
	TextShader.SetUniformVec3("textColor", color);
	GLCall(glActiveTexture(GL_TEXTURE0));
	glBindVertexArray(VAO);
	int l = wcslen(text);
	for (int i = 0;i < l;i++) {
		wchar_t c = text[i];
		if (Characters.find(c) == Characters.end()) 
			Load(Filepath, FontSize, c, GL_TRUE);
		
		Character ch = Characters[c];
		if(c == '生')std::cout << ch.Advance << std::endl;

		GLfloat xpos = x + ch.Bearing.x * scale;
		//H为字形空间的顶部，用来计算垂直偏移
		GLfloat ypos = y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 },
			{ xpos,     ypos,       0.0, 0.0 },

			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 }
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof vertices, vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
