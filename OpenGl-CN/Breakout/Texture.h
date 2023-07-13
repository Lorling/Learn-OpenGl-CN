#pragma once

#include "Renderer.h"

class Texture2D {
private:
    GLuint ID;
    GLuint Width, Height;
    GLuint Internal_Format; 
    GLuint Image_Format;
    GLuint Wrap_S; 
    GLuint Wrap_T; 
    GLuint Filter_Min;
    GLuint Filter_Max;
public:
    Texture2D();
    void Generate(GLuint width, GLuint height, unsigned char* data);
    void Bind() const;
    GLuint& GetID() { return ID; } ;
    void SetFormat(GLuint format);
};