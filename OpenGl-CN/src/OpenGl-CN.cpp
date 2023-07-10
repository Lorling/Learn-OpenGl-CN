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
#include <vector>
#include <map>

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
    //将函数注册到window的回调函数中
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //在调用这个函数之后，无论我们怎么去移动鼠标，光标都不会显示了，它也不会离开窗口。
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //开启深度测试，根据z值可以让距离摄像机更近的不会被更远的覆盖
    GLCall(glEnable(GL_DEPTH_TEST));
    //设置深度测试的运算比较符为在片段深度值小于缓冲的深度值时通过测试
    GLCall(glDepthFunc(GL_LESS));
    //开启模板测试
    GLCall(glEnable(GL_STENCIL_TEST));
    //设置所有片段都会更新模板缓冲值，并且值设置为1
    GLCall(glStencilFunc(GL_ALWAYS, 1, 0x00FF));
    //第一个参数为模板测试失败时采取的行为，第二个参数为模板测试通过，但深度测试失败时采取的行为，第三个参数为模板测试和深度测试都通过时采取的行为，replace为将模板值设置为glStencilFunc函数设置的ref值。
    GLCall(glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE));
    //开启混合
    GLCall(glEnable(GL_BLEND));
    //设定混合函数
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));//使用线框来绘制三角形
    //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//切换回默认模式
    //交换间隔，交换缓冲区之前等待的帧数，通常称为v-sync,默认情况下，交换间隔为0,这里设置为1，即每帧更新一次
    glfwSwapInterval(1);
    //输出版本
    std::cout << glGetString(GL_VERSION) << std::endl;
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);//宽和高从之前设置的窗口中获得
    glViewport(0, 0, width, height);//前两个参数为窗口左下角的位置，多两个为宽和高
    //翻转图像y轴，因为图像0.0坐标通常在左上角，但是OpenGL0.0坐标在左下角
    stbi_set_flip_vertically_on_load(true);
    {

        float cubeVertices[] = {
            // 位置          // 渲染坐标
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };
        float planeVertices[] = {
                                    //可以使地板纹理重复
             5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

             5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
             5.0f, -0.5f, -5.0f,  2.0f, 2.0f
        };
        float transparentVertices[] = {
            0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  0.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  0.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  1.0f
        };

        std::vector<glm::vec3> vegetation;
        vegetation.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
        vegetation.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
        vegetation.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
        vegetation.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
        vegetation.push_back(glm::vec3(0.5f, 0.0f, -0.6f));

        unsigned int vao, vbo;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glGenBuffers(1, &vbo));
        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof cubeVertices, &cubeVertices, GL_STATIC_DRAW));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof GL_FLOAT, 0));
        GLCall(glEnableVertexAttribArray(1));
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof GL_FLOAT, (void*)(3 * sizeof GL_FLOAT)));
        GLCall(glBindVertexArray(0));

        unsigned int planevao, planevbo;
        GLCall(glGenVertexArrays(1, &planevao));
        GLCall(glGenBuffers(1, &planevbo));
        GLCall(glBindVertexArray(planevao));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, planevbo));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof planeVertices, &planeVertices,  GL_STATIC_DRAW));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof GL_FLOAT, 0));
        GLCall(glEnableVertexAttribArray(1));
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof GL_FLOAT, (void*)(3 * sizeof GL_FLOAT)));
        GLCall(glBindVertexArray(0));

        unsigned int grassvao, grassvbo;
        GLCall(glGenVertexArrays(1, &grassvao));
        GLCall(glGenBuffers(1, &grassvbo));
        GLCall(glBindVertexArray(grassvao));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, grassvbo));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof transparentVertices, &transparentVertices, GL_STATIC_DRAW));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof GL_FLOAT, 0));
        GLCall(glEnableVertexAttribArray(1));
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof GL_FLOAT, (void*)(3 * sizeof GL_FLOAT)));
        GLCall(glBindVertexArray(0));

        unsigned int cubeTexture = loadTexture("res/textures/marble.jpg");
        unsigned int planeTexture = loadTexture("res/textures/metal.png");
        unsigned int grassTexture = loadTexture("res/textures/blending_transparent_window.png");

        Shader cubeShader("src/shaders/Basic.shader");
        Shader singleShader("src/shaders/StencilSingle.shader");

        cubeShader.Use();
        cubeShader.SetUniformInt("texture1", 0);

        //循环直到用户退出窗口
        while (!glfwWindowShouldClose(window)) {
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            //清空上一次的渲染结果
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//可以指定使用该颜色来在清空之后 填充
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));//GL_DEPTH_BUFFER_BIT为清除深度缓冲

            key_callback(window);

            cubeShader.Use();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetFov()), (float)width / height, 0.1f, 100.0f);
            cubeShader.SetUniformMatrix4fv("view", view);
            cubeShader.SetUniformMatrix4fv("projection", projection);

            cubeShader.Use();
            cubeShader.SetUniformMatrix4fv("view", view);
            cubeShader.SetUniformMatrix4fv("projection", projection);

            //确保在绘制地板时不会更新模板缓冲
            GLCall(glStencilMask(0x00));
            GLCall(glBindVertexArray(planevao));
            GLCall(glActiveTexture(GL_TEXTURE0));
            GLCall(glBindTexture(GL_TEXTURE_2D, planeTexture));
            cubeShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
            GLCall(glBindVertexArray(0));

            GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
            GLCall(glStencilMask(0xFF));
            //绘制方块
            GLCall(glBindVertexArray(vao));
            GLCall(glBindTexture(GL_TEXTURE_2D, cubeTexture));
            model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
            cubeShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
            cubeShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

            //绘制窗户
            std::map<float, glm::vec3> sorted;
            for (unsigned int i = 0;i < vegetation.size();i++) {
                float distance = glm::length(camera.GetPosition() - vegetation[i]);
                sorted[distance] = vegetation[i];
            }
            GLCall(glBindVertexArray(grassvao));
            GLCall(glBindTexture(GL_TEXTURE_2D, grassTexture));
            for (auto i = sorted.rbegin();i != sorted.rend();i++) {
                model = glm::mat4(1.0f);
                model = glm::translate(model, i->second);
                cubeShader.Use();
                cubeShader.SetUniformMatrix4fv("model", model);
                GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
            }
            
            GLCall(glBindVertexArray(0));

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

unsigned int loadTexture(const char* filepath) {
    unsigned int id;
    GLCall(glGenTextures(1, &id));

    int width, height, nrComponents;
    unsigned char* data = stbi_load(filepath, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum fromat;
        if (nrComponents == 1) fromat = GL_RED;
        else if (nrComponents == 3) fromat = GL_RGB;
        else if (nrComponents == 4) fromat = GL_RGBA;

        GLCall(glBindTexture(GL_TEXTURE_2D, id));
        GLCall(glTexImage2D(GL_TEXTURE_2D, 0, fromat, width, height, 0, fromat, GL_UNSIGNED_BYTE, data));
        GLCall(glGenerateMipmap(GL_TEXTURE_2D));

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//如果透明图片边缘存在有色边框的话，可以通过改变环绕方式解决
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//因为图片的边缘会进行插值，这样就会得到一个不是透明的值
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Texture failed to load at path : " << filepath << std::endl;
    }
    stbi_image_free(data);

    return id;
}