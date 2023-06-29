#include "Shader.h"

#include "Renderer.h"

#include <fstream>
#include <sstream>
#include <string>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

//��·����ָ���*.shaderԴ���еĶ����ɫ�����
static ShaderProgramSource ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {//npos��һ����Ч���ַ���λ��
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

//����shaderԴ��
static GLuint CompileShader(GLuint type, const std::string& source) {
    GLCall(GLuint id = glCreateShader(type));//������Ӧ���͵���ɫ��
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));//������ɫ��Դ�룬��һ������ΪҪ�������ɫ�����󣬵ڶ�������ΪԴ���ַ���������������ΪԴ��
    GLCall(glCompileShader(id));//������ɫ��

    //���������
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));//��ȡ��ǰ��ɫ������״̬
    if (result == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));//��ȡ��־����
        char* message = (char*)alloca(length * sizeof(char));//������־����Ϊ��Ϣ�����ַ
        GLCall(glGetShaderInfoLog(id, length, &length, message));//��ȡ��־��Ϣ
        std::cout << "Failed to " << (type == GL_VERTEX_SHADER ? "vertex" : "fargment") << " Compile Shader !" << std::endl;
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));//ɾ����ɫ��
        return 0;
    }

    return id;
}

//����shader����
static GLuint CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(GLuint program = glCreateProgram());//��������
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //����ɫ�����ӵ�������
    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));//���ӳ���
    GLCall(glValidateProgram(program));//��֤

    //ɾ����ɫ������Ϊ�����Ѿ������ӵ��˳������棬�������ǲ��ٱ���Ҫ��
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
    GLCall(glDeleteProgram(Program));//ɾ����ɫ������
}

void Shader::Use()
{
    GLCall(glUseProgram(Program));
}

void Shader::Unuse()
{
    GLCall(glUseProgram(0));
}
