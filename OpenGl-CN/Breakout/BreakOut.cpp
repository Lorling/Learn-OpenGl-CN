#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Game.h"

#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

Game BreakOut(800, 600);

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

    GLCall(glfwSetKeyCallback(window, key_callback));

    GLCall(glViewport(0, 0, 800, 600));//第一个和第二个指定了屏幕的左下角位置

    GLCall(glEnable(GL_CULL_FACE));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    BreakOut.Init();

    GLfloat deltaTime = 0.0f;//当前帧与上一帧的时间差
    GLfloat lastFrame = 0.0f;//上一帧的时间

    while (!glfwWindowShouldClose(window)) {
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();

        BreakOut.ProccessInput(deltaTime);

        BreakOut.Update(deltaTime);

        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));

        BreakOut.Render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    //如果用户按下esc键，将窗口的WindowShouldClose，既关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {//按下
            BreakOut.Keys[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            BreakOut.Keys[key] = false;
        }
    }
}