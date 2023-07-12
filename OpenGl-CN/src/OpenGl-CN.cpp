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
unsigned int loadCubemap(std::vector<std::string> faces);//加载立方体贴图

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
    GLCall(glDepthFunc(GL_LEQUAL));
    //开启模板测试
    //GLCall(glEnable(GL_STENCIL_TEST));
    //设置所有片段都会更新模板缓冲值，并且值设置为1
    //GLCall(glStencilFunc(GL_ALWAYS, 1, 0x00FF));
    //第一个参数为模板测试失败时采取的行为，第二个参数为模板测试通过，但深度测试失败时采取的行为，第三个参数为模板测试和深度测试都通过时采取的行为，replace为将模板值设置为glStencilFunc函数设置的ref值。
    //GLCall(glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE));
    //开启混合
    GLCall(glEnable(GL_BLEND));
    //设定混合函数
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //开启面剔除
    //GLCall(glEnable(GL_CULL_FACE));
    //设置正向面为顺时针，默认值为GL_CCW逆时针
    //GLCall(glFrontFace(GL_CW));
    //设置剔除的面,默认为背向面GL_BACK,FRONT为前向
    //GLCall(glCullFace(GL_FRONT));
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
    //stbi_set_flip_vertically_on_load(true);
    {
        float cubeVertices[] = {
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
        };
        float skyboxVertices[] = {
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };
        std::vector<std::string> faces = {
            "res/textures/skybox/right.jpg",
            "res/textures/skybox/left.jpg",
            "res/textures/skybox/top.jpg",
            "res/textures/skybox/bottom.jpg",
            "res/textures/skybox/front.jpg",
            "res/textures/skybox/back.jpg"
        };

        unsigned int vao, vbo;
        GLCall(glGenVertexArrays(1, &vao));
        GLCall(glGenBuffers(1, &vbo));
        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof cubeVertices, &cubeVertices, GL_STATIC_DRAW));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof GL_FLOAT, 0));
        GLCall(glEnableVertexAttribArray(1));
        GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof GL_FLOAT, (void*)(3 * sizeof GL_FLOAT)));
        GLCall(glBindVertexArray(0));

        unsigned int skyvao, skyvbo;
        GLCall(glGenVertexArrays(1, &skyvao));
        GLCall(glGenBuffers(1, &skyvbo));
        GLCall(glBindVertexArray(skyvao));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, skyvbo));
        GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof skyboxVertices, &skyboxVertices, GL_STATIC_DRAW));
        GLCall(glEnableVertexAttribArray(0));
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof GL_FLOAT, 0));
        GLCall(glBindVertexArray(0));

        unsigned int skyTexture = loadCubemap(faces);

        Shader cubeShader("src/shaders/Basic.shader");
        Shader skyShader("src/shaders/sky.shader");
        Shader modelShader("src/shaders/Light.shader");

        skyShader.Use();
        skyShader.SetUniformInt("texture1", 0);

        cubeShader.Use();
        cubeShader.SetUniformInt("texture1", 0);

        Shader redShader("src/shaders/red.shader");
        Shader greenShader("src/shaders/green.shader");
        Shader blueShader("src/shaders/blue.shader");
        Shader yellowShader("src/shaders/yellow.shader");

        //获取uniform块的索引
        unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(redShader.GetProgramID(), "Matrices");
        unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(greenShader.GetProgramID(), "Matrices");
        unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(blueShader.GetProgramID(), "Matrices");
        unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(yellowShader.GetProgramID(), "Matrices");

        //将每个uniform块设置位绑定点0
        glUniformBlockBinding(redShader.GetProgramID(), uniformBlockIndexRed, 0);
        glUniformBlockBinding(greenShader.GetProgramID(), uniformBlockIndexGreen, 0);
        glUniformBlockBinding(blueShader.GetProgramID(), uniformBlockIndexBlue, 0);
        glUniformBlockBinding(yellowShader.GetProgramID(), uniformBlockIndexYellow, 0);

        //创建一个uniform缓冲对象
        unsigned int ubo;
        GLCall(glGenBuffers(1, &ubo));
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, ubo));
        GLCall(glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof glm::mat4, NULL, GL_STATIC_DRAW));//创建一个空的内存，大小为两个mat4
        GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));
        //将uniform缓冲对象绑定到绑定点0
        glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof glm::mat4);

        //循环直到用户退出窗口
        while (!glfwWindowShouldClose(window)) {
            //计算每帧时间
            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            //输入
            key_callback(window);

            //清空上一次的渲染结果
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//可以指定使用该颜色来在清空之后 填充
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));//GL_DEPTH_BUFFER_BIT为清除深度缓冲

            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = camera.GetViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera.GetFov()), (float)width / height, 0.1f, 100.0f);
            GLCall(glBindBuffer(GL_UNIFORM_BUFFER, ubo));
            //将两个矩阵填充进缓冲中
            glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof glm::mat4, glm::value_ptr(projection));//第二个为偏移量，第三个数据大小，第四个为数据本身
            glBufferSubData(GL_UNIFORM_BUFFER, sizeof glm::mat4, sizeof glm::mat4, glm::value_ptr(view));
            GLCall(glBindBuffer(GL_UNIFORM_BUFFER, 0));

            GLCall(glBindVertexArray(vao));
            redShader.Use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f));
            redShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
            greenShader.Use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f));
            greenShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
            blueShader.Use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f));
            blueShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
            yellowShader.Use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f));
            yellowShader.SetUniformMatrix4fv("model", model);
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
            GLCall(glBindVertexArray(0));
            
            GLCall(glDepthMask(GL_FALSE));
            skyShader.Use();
            skyShader.SetUniformMatrix4fv("view", glm::mat4(glm::mat3(camera.GetViewMatrix())));
            skyShader.SetUniformMatrix4fv("projection", projection);
            GLCall(glBindVertexArray(skyvao));
            GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, skyTexture));
            GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
            GLCall(glDepthMask(GL_TRUE));

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

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int id;
    GLCall(glGenTextures(1, &id));
    GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, id));

    int width, height, numberChannels;
    for (unsigned int i = 0;i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &numberChannels, 0);
        if (data) {
            GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
        }
        else {
            std::cout << "fail at " << faces[i] << std::endl;
        }
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}