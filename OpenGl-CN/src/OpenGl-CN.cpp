#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "../res/stb_image/stb_image.h"
#include "Camera.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间

//灯光在世界坐标中的位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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

    std::cout << glGetString(GL_VERSION) << std::endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);//宽和高从之前设置的窗口中获得
    glViewport(0, 0, width, height);//前两个参数为窗口左下角的位置，多两个为宽和高

    //将函数注册到window的回调函数中
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    {
        //顶点位置，浮点型数组
        GLfloat position[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f
        };

        GLuint indics[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9, 10, 10, 11, 8,
            12, 13, 14, 14 ,15, 12,
            16, 17, 18, 18, 19, 16,
            20, 21, 22, 22, 23, 20
        };

        glm::vec3 cubePositions[] = {
            glm::vec3(0.0f,  0.0f,  0.0f),
            glm::vec3(2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3(2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3(1.3f, -2.0f, -2.5f),
            glm::vec3(1.5f,  2.0f, -2.5f),
            glm::vec3(1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
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
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0));//设置顶点属性,第一个参数为位置值，第二个为属性数量，第三个为参数类型，第四个为是否标准化，第五个为步长，第六个为起始位置
        GLCall(glEnableVertexAttribArray(1));//激活顶点属性，索引0位置
        GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(float))));
        GLCall(glEnableVertexAttribArray(2));//激活顶点属性，索引0位置
        GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(float))));

        GLuint ibo;
        GLCall(glGenBuffers(1, &ibo));//生成索引缓冲区
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//绑定索引缓冲区
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indics, indics, GL_STATIC_DRAW));//设置索引缓冲区数据

        GLuint lightvao;//顶点数组对象
        GLCall(glGenVertexArrays(1, &lightvao));
        GLCall(glBindVertexArray(lightvao));
        // 只需要绑定buffer不用再次设置buffer的数据，因为箱子的buffer数据中已经包含了正确的立方体顶点数据
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));//绑定缓冲区

        GLCall(glEnableVertexAttribArray(0));//激活顶点属性，索引0位置
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0));//设置顶点属性,第一个参数为位置值，第二个为属性数量，第三个为参数类型，第四个为是否标准化，第五个为步长，第六个为起始位置
        //ibo与buffer同理
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//绑定索引缓冲区

        unsigned int diffuseMap = loadTexture("res/textures/container2.png");
        unsigned int specularMap = loadTexture("res/textures/container2_specular.png");

        Shader shader("res/shaders/Basic.shader");
        Shader lightShader("res/shaders/Light.shader");
        shader.Use();

        shader.SetUniformInt("material.diffuse", 0);
        shader.SetUniformInt("material.specular", 1);

        GLCall(glBindVertexArray(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        shader.Unuse();
        //循环直到用户退出窗口
        while (!glfwWindowShouldClose(window)) {
            //清空上一次的渲染结果
            //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//可以指定使用该颜色来在清空之后 填充
            GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));//GL_DEPTH_BUFFER_BIT为清除深度缓冲

            float currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            key_callback(window);

            shader.Use();
            GLCall(glBindVertexArray(vao));

            
            //观察矩阵，使物体向移动场景的反方向移动
            glm::mat4 view = camera.GetViewMatrix();
            //投影矩阵，使物体按透视的方法变换到裁剪坐标
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(camera.GetFov()), (float)width / height, 0.1f, 100.0f);

            shader.SetUniformMatrix4fv("view", view);
            shader.SetUniformMatrix4fv("projection", projection);
            shader.SetUniformVec3("viewPos", camera.GetPosition());
            shader.SetUniformVec3("light.position", camera.GetPosition());
            shader.SetUniformVec3("light.direction", camera.GetFront());

            shader.SetUniformFloat("material.shininess", 32.0f);
            shader.SetUniformFloat("light.cutoff", glm::cos(glm::radians(12.5f)));
            shader.SetUniformFloat("light.outerCutoff", glm::cos(glm::radians(17.5f)));
            shader.SetUniformFloat("light.constant", 1.0f);
            shader.SetUniformFloat("light.linear", 0.09f);
            shader.SetUniformFloat("light.quadratic", 0.032f);

            glm::vec3 lightColor = glm::vec3(1.0f);

            shader.SetUniformVec3("light.ambient", lightColor* glm::vec3(0.2f));
            shader.SetUniformVec3("light.diffuse", lightColor* glm::vec3(0.5f));
            shader.SetUniformVec3("light.specular", lightColor* glm::vec3(1.0f));

            GLCall(glActiveTexture(GL_TEXTURE0));
            GLCall(glBindTexture(GL_TEXTURE_2D, diffuseMap));
            GLCall(glActiveTexture(GL_TEXTURE1));
            GLCall(glBindTexture(GL_TEXTURE_2D, specularMap));

            for (unsigned int i = 0;i < 10;i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angle = 20.0f * i;
                model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                shader.SetUniformMatrix4fv("model", model);

                GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));//第一个参数为绘制的模式，第二个参数为绘制顶点的数量，第三个参数为索引的类型，第四个为偏移量（或者传递一个索引数组，但是这是当你不在使用索引缓冲对象的时候）
            }

            lightShader.Use();
            GLCall(glBindVertexArray(lightvao));

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, lightPos);
            model = glm::scale(model, glm::vec3(0.2f));//将灯光的体积缩小

            lightShader.SetUniformMatrix4fv("model", model);
            lightShader.SetUniformMatrix4fv("view", view);
            lightShader.SetUniformMatrix4fv("projection", projection);

            GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));

            //交换前缓冲区和后缓冲区，因为如果使用单缓冲区的话，生成的图像需要一步一步的生成出来，看起来不真实，使用双缓冲区的话，前缓冲区为屏幕上显示的图像，后缓冲区为正在渲染的图像，渲染完成之后将两个缓冲区交换，这样可以消除不真实感。
            glfwSwapBuffers(window);

            //检测有没有触发键盘输入、鼠标移动之类的函数，并调用对应的回调函数。 既轮询和处理事件
            glfwPollEvents();
        }
        GLCall(glDeleteVertexArrays(1, &vao));
        GLCall(glDeleteVertexArrays(1, &lightvao));
        GLCall(glDeleteBuffers(1, &buffer));
        GLCall(glDeleteBuffers(1, &ibo));
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

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        std::cout << "Texture failed to load at path : " << filepath << std::endl;
    }
    stbi_image_free(data);

    return id;
}