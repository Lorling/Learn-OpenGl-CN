#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "../res/stb_image/stb_image.h"
#include "Camera.h"
#include "Model.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间

//灯光在世界坐标中的位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 dirLightDir(-0.2f, -1.0f, -0.3f);

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

void key_callback(GLFWwindow* window);//键盘回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos);//鼠标回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);//滚轮回调函数
unsigned int loadTexture(const char* filepath);

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
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //在调用这个函数之后，无论我们怎么去移动鼠标，光标都不会显示了，它也不会离开窗口。
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //开启深度测试，根据z值可以让距离摄像机更近的不会被更远的覆盖
    GLCall(glEnable(GL_DEPTH_TEST));
    //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));//使用线框来绘制三角形
    //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//切换回默认模式
    //交换间隔，交换缓冲区之前等待的帧数，通常称为v-sync,默认情况下，交换间隔为0,这里设置为1，即每帧更新一次
    glfwSwapInterval(1);
    //输出版本
    std::cout << glGetString(GL_VERSION) << std::endl;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);//宽和高从之前设置的窗口中获得
    glViewport(0, 0, width, height);//前两个参数为窗口左下角的位置，多两个为宽和高
    //将函数注册到window的回调函数中
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //翻转图像y轴，因为图像0.0坐标通常在左上角，但是OpenGL0.0坐标在左下角
    stbi_set_flip_vertically_on_load(true);
    {
        Shader shader("res/shaders/Basic.shader");
        Model Model("res/objects/nanosuit/nanosuit.obj");
        //循环直到用户退出窗口
        while (!glfwWindowShouldClose(window)) {
            //清空上一次的渲染结果
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//可以指定使用该颜色来在清空之后 填充
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));//GL_DEPTH_BUFFER_BIT为清除深度缓冲

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            key_callback(window);

            shader.Use();
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetFov()), (float)width / height, 0.1f, 100.0f);
            shader.SetUniformMatrix4fv("model", model);
            shader.SetUniformMatrix4fv("view", view);
            shader.SetUniformMatrix4fv("projection", projection);
            shader.SetUniformVec3("dirLight.direction", dirLightDir);
            shader.SetUniformVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
            shader.SetUniformVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
            shader.SetUniformVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
            shader.SetUniformVec3("pointLights[0].position", 1.0f,0.0f,0.0f);
            shader.SetUniformVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
            shader.SetUniformVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
            shader.SetUniformVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
            shader.SetUniformFloat("pointLights[0].constant", 1.0f);
            shader.SetUniformFloat("pointLights[0].linear", 0.09f);
            shader.SetUniformFloat("pointLights[0].quadratic", 0.032f);

            Model.Draw(shader);

            //交换前缓冲区和后缓冲区，因为如果使用单缓冲区的话，生成的图像需要一步一步的生成出来，看起来不真实，使用双缓冲区的话，前缓冲区为屏幕上显示的图像，后缓冲区为正在渲染的图像，渲染完成之后将两个缓冲区交换，这样可以消除不真实感。
            glfwSwapBuffers(window);

            //检测有没有触发键盘输入、鼠标移动之类的函数，并调用对应的回调函数。 既轮询和处理事件
            glfwPollEvents();
        }
    }
    glfwTerminate();//释放glfw分配的资源
    return 0;
}

void key_callback(GLFWwindow* window) {
    //如果用户按下esc键，将窗口的WindowShouldClose，既关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    //如果用户按下WSAD，就将摄像机向相应方向移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyborad(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyborad(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyborad(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyborad(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    camera.MouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.MoustScroll(yoffset);
}