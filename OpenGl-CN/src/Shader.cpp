#include "Shader.h"

#include "Renderer.h"

#include <fstream>
#include <sstream>
#include <string>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

//将路径所指向的*.shader源码中的多个着色器拆分
static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {//npos是一个无效的字符串位置
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else if (type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

//编译shader源码
static GLuint CompileShader(GLuint type, const std::string& source) {
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

//创建shader程序
static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(GLuint program = glCreateProgram());//创建程序
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //讲着色器附加到程序上
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));//链接程序
    GLCall(glValidateProgram(program));//验证

    //删除着色器，因为他们已经被链接到了程序里面，所以他们不再被需要了
    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

Shader::Shader(const std::string& filepath)
{
    ShaderProgramSource source = ParseShader(filepath);
    Program = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    GLCall(glDeleteProgram(Program));//删除着色器程序
}

void Shader::Use()
{
    GLCall(glUseProgram(Program));
}

void Shader::Unuse()
{
    GLCall(glUseProgram(0));
}
