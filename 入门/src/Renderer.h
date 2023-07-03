#pragma once

#include <glad/glad.h>

#define ASSERT(x) if(!(x)) __debugbreak()
#define GLCall(x) GLClearError(); x; ASSERT(GLLogCall(#x, __FILE__, __LINE__))

//将调用这个函数之前的错误清空
void GLClearError();

//输出错误信息
bool GLLogCall(const char* function, const char* file, int line);