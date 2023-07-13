#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Game.h"

#include <iostream>

void key_callback(GLFWwindow* window);//键盘回调函数

int main() 
{
	glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//设置主版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);//设置次版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//使用核心模式
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);//禁止用户调整窗口大小

    GLFWwindow* window = glfwCreateWindow(800, 600, "BreakOut", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    GLCall(glViewport(0, 0, 800, 600));//第一个和第二个指定了屏幕的左下角位置

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    Game BreakOut(800, 600);
    BreakOut.Init();

    GLfloat deltaTime = 0.0f;//当前帧与上一帧的时间差
    GLfloat lastFrame = 0.0f;//上一帧的时间

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        key_callback(window);

        BreakOut.ProccessInput(deltaTime);

        BreakOut.Update(deltaTime);

        GLCall(glClearColor(0.0f, 0.0f, 0.0f,1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        BreakOut.Render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window) {
    //如果用户按下esc键，将窗口的WindowShouldClose，既关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}