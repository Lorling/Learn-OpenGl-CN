#include "Shader.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

Shader::Shader(const std::string& filepath)
{
    GLCall(Program = glCreateProgram());//创建程序
    ParseShader(filepath);
}

Shader::~Shader()
{
    //在资源管理中进行删除
    //GLCall(glDeleteProgram(Program));//删除着色器程序
}

void Shader::Use()
{
    GLCall(glUseProgram(Program));
}

void Shader::Unuse()
{
    GLCall(glUseProgram(0));
}

void Shader::ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    std::vector<ShaderType> v;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {//npos是一个无效的字符串位置
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
            if (ss[(int)type].str().size()) {
                CreateShader(type, ss[(int)type].str());
            }
            ss[(int)type].clear();
            v.push_back(type);
        }
        else if (type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }
    for (auto i : v) {
        if (i == ShaderType::NONE) continue;
        if (ss[(int)i].str().size()) {
            CreateShader(i, ss[(int)i].str());
        }
    }
}

GLuint Shader::CompileShader(GLuint type, const std::string& source) {
    GLCall(GLuint id = glCreateShader(type));//创建对应类型的着色器
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));//设置着色器源码，第一个参数为要编译的着色器对象，第二个参数为源码字符串数量，第三个为源码
    GLCall(glCompileShader(id));//编译着色器

    //编译错误处理
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));//获取当前着色器编译状态
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));//获取日志长度
        char* message = (char*)alloca(length * sizeof(char));//根据日志长度为信息分配地址
        GLCall(glGetShaderInfoLog(id, length, &length, message));//获取日志信息
        std::cout << "Failed to " << (type == GL_VERTEX_SHADER ? "vertex" : "fargment") << " Compile Shader !" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));//删除着色器
        return 0;
    }

    return id;
}

void Shader::CreateShader(ShaderType& type, const std::string& sourceShader) {
    GLuint shader;
    if(type == ShaderType::VERTEX) shader = CompileShader(GL_VERTEX_SHADER, sourceShader);
    if(type == ShaderType::FRAGMENT) shader = CompileShader(GL_FRAGMENT_SHADER, sourceShader);

    //讲着色器附加到程序上
    GLCall(glAttachShader(Program, shader));
    GLCall(glLinkProgram(Program));//链接程序
    GLCall(glValidateProgram(Program));//验证

    //删除着色器，因为他们已经被链接到了程序里面，所以他们不再被需要了
    GLCall(glDeleteShader(shader));
}

void Shader::SetUniformVec2(const char* uniformName, float f1, float f2)
{
    GLCall(glUniform2f(glGetUniformLocation(Program, uniformName), f1, f2));
}

void Shader::SetUniformVec2(const char* uniformName, glm::vec2 vector)
{
    GLCall(glUniform2f(glGetUniformLocation(Program, uniformName), vector.x, vector.y));
}

void Shader::SetUniformVec3(const char* uniformName, float f1, float f2, float f3)
{
    GLCall(glUniform3f(glGetUniformLocation(Program, uniformName), f1, f2, f3));
}

void Shader::SetUniformVec3(const char* uniformName, glm::vec3 vector)
{
    GLCall(glUniform3f(glGetUniformLocation(Program, uniformName), vector.x, vector.y, vector.z));
}

void Shader::SetUniformVec4(const char* uniformName, float f1, float f2, float f3, float f4)
{
    GLCall(glUniform4f(glGetUniformLocation(Program, uniformName), f1, f2, f3, f4));
}

void Shader::SetUniformVec4(const char* uniformName, glm::vec4 vector)
{
    GLCall(glUniform4f(glGetUniformLocation(Program, uniformName), vector.x, vector.y, vector.z, vector.w));
}

void Shader::SetUniformMatrix4fv(const char* uniformName, glm::mat4 matrix)
{
    GLCall(glUniformMatrix4fv(glGetUniformLocation(Program, uniformName), 1, GL_FALSE, glm::value_ptr(matrix)));
}

void Shader::SetUniformFloat(const char* uniformName, float f)
{
    GLCall(glUniform1f(glGetUniformLocation(Program, uniformName), f));
}

void Shader::SetUniformInt(const char* uniformName, int t)
{
    GLCall(glUniform1i(glGetUniformLocation(Program, uniformName), t));
}
