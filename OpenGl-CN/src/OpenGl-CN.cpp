#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"
#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    //如果用户按下esc键，将窗口的WindowShouldClose，既关闭窗口
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main(void)
{
    glfwInit();//初始化GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置主版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置次版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//禁止用户调整窗口大小

    //创建一个窗口对象
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    //将窗口的上下文设置为当前上下文
    glfwMakeContextCurrent(window);

    //交换间隔，交换缓冲区之前等待的帧数，通常称为v-sync,默认情况下，交换间隔为0,这里设置为1，即每帧更新一次
    glfwSwapInterval(10);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Error glew"<< std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);//宽和高从之前设置的窗口中获得
    glViewport(0, 0, width, height);//前两个参数为窗口左下角的位置，多两个为宽和高

    //将函数注册到window的回调函数中
    glfwSetKeyCallback(window, key_callback);

    {
        //顶点位置，浮点型数组
        GLfloat position[20] = {
            -0.5f, -0.5f,1.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,0.0f, 1.0f, 0.0f,
             0.5f,  0.5f,1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,0.0f, 0.0f, 1.0f
        };

        GLuint indics[6] = {
            0, 1, 2,
            2, 3, 0
        };

        //如果是兼容性配置文件的话，会默认创建一个，但是核心配置文件需要自己创建
        GLuint vao;//顶点数组对象
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glBindVertexArray(vao));

        GLuint buffer;//顶点缓冲对象
        GLCall(glGenBuffers(1, &buffer));//生成缓冲区
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));//绑定缓冲区
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof position, position, GL_STATIC_DRAW));//设置缓冲区数据

        GLCall(glEnableVertexAttribArray(0));//激活顶点属性，索引0位置
        GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0));//设置顶点属性,第一个参数为位置值，第二个为属性数量，第三个为参数类型，第四个为是否标准化，第五个为步长，第六个为起始位置
        GLCall(glEnableVertexAttribArray(1));//激活顶点属性，索引1位置
        GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat))));//设置顶点属性

        GLuint ibo;
        GLCall(glGenBuffers(1, &ibo));//生成索引缓冲区
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//绑定索引缓冲区
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indics, indics, GL_STATIC_DRAW));//设置索引缓冲区数据

        Shader shader("res/shaders/Basic.shader");
        shader.Use();

        //获取u_color的location并进行更改
        GLCall(int location = glGetUniformLocation(shader.GetProgramID(), "u_color"));//查询uniform位置不需要先使用着色器程序
        ASSERT(location != -1);//返回值-1表示没有找到uniform
        GLCall(glUniform4f(location, 1.0f, 0.2f, 0.8f, 1.0f));//但是更改uniform值必须先使用着色器程序

        GLCall(glBindVertexArray(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        shader.Unuse();

        //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));//使用线框来绘制三角形
        //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//切换回默认模式

        float r = 1.0f;
        float increment = 0.05f;

        //循环直到用户退出窗口
        while (!glfwWindowShouldClose(window)) {
            //清空上一次的渲染结果
            //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//可以指定使用该颜色来在清空之后 填充
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            shader.Use();
            GLCall(glBindVertexArray(vao));

            //绘制图形
            GLCall(glUniform4f(location, r, 0.2f, 0.8f, 1.0f));
            GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));//第一个参数为绘制的模式，第二个参数为绘制顶点的数量，第三个参数为索引的类型，第四个为偏移量（或者传递一个索引数组，但是这是当你不在使用索引缓冲对象的时候）

            if (r >= 1.f) increment = -0.05f;
            else if (r <= 0.0f) increment = 0.05f;
            r += increment;

            //交换前缓冲区和后缓冲区，因为如果使用单缓冲区的话，生成的图像需要一步一步的生成出来，看起来不真实，使用双缓冲区的话，前缓冲区为屏幕上显示的图像，后缓冲区为正在渲染的图像，渲染完成之后将两个缓冲区交换，这样可以消除不真实感。
            glfwSwapBuffers(window);

            //检测有没有触发键盘输入、鼠标移动之类的函数，并调用对应的回调函数。 既轮询和处理事件
            glfwPollEvents();
        }
    }
    glfwTerminate();//释放glfw分配的资源
    return 0;
}