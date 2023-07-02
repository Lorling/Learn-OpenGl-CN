#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "Shader.h"
#include "../res/stb_image/stb_image.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间

void key_callback(GLFWwindow* window) {
    //如果用户按下esc键，将窗口的WindowShouldClose，既关闭窗口
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    //如果用户按下WSAD，就将摄像机向相应方向移动
    float cameraSpeed = 1.5f * deltaTime;//摄像机移动速度
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront,cameraUp));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
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
    glfwSwapInterval(1);

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
    //glfwSetKeyCallback(window, key_callback);

    {
        //顶点位置，浮点型数组
        GLfloat position[] = {
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
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
        GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0));//设置顶点属性,第一个参数为位置值，第二个为属性数量，第三个为参数类型，第四个为是否标准化，第五个为步长，第六个为起始位置
        GLCall(glEnableVertexAttribArray(1));//激活顶点属性，索引1位置
        GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))));//设置顶点属性

        GLCall(glActiveTexture(GL_TEXTURE0));//设置当前纹理单元为0

        GLuint texture1,texture2;
        GLCall(glGenTextures(1, &texture1));//生成纹理
        GLCall(glGenTextures(1, &texture2));//生成纹理
        GLCall(glBindTexture(GL_TEXTURE_2D, texture1));//绑定纹理

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);//设置纹理水平环绕方式为镜像
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);//纹理垂直环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);//设置缩小过滤的方式为多级渐远纹理
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//设置放大过滤的方式为线性过滤  邻近过滤会存在颗粒感，线性过滤更平滑

        int width, height, nrChannels;//三个分别为宽，高，颜色通道个数
        stbi_set_flip_vertically_on_load(true);//翻转图像y轴，因为图像0.0坐标通常在左上角，但是OpenGL0.0坐标在左下角
        unsigned char* data = stbi_load("res/textures/container.jpg", &width, &height, &nrChannels, 0);

        if (data) {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));//根据之前的图片生成纹理，第一个参数为纹理目标，第二个参数为纹理指定多级渐远纹理的级别（0是基本级别），第三个参数为纹理存储格式（这里为jpg，只有RGB值），第四个和第五个参数设置最终的纹理的宽度和高度，第六个总是0，第七第八个参数定义了源图的格式和数据类型，最后一个为真正的图像数据
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));//为当前绑定的纹理自动生成所有需要的多级渐远纹理
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);//绑定到纹理之后释放图像的内存

        GLCall(glActiveTexture(GL_TEXTURE1));//设置当前纹理单元为1
        GLCall(glBindTexture(GL_TEXTURE_2D, texture2));//绑定纹理

        data = stbi_load("res/textures/awesomeface.png", &width, &height, &nrChannels, 0);

        if (data) {
            GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));//根据之前的图片生成纹理，第一个参数为纹理目标，第二个参数为纹理指定多级渐远纹理的级别（0是基本级别），第三个参数为纹理存储格式（这里为jpg，只有RGB值），第四个和第五个参数设置最终的纹理的宽度和高度，第六个总是0，第七第八个参数定义了源图的格式和数据类型，最后一个为真正的图像数据
            GLCall(glGenerateMipmap(GL_TEXTURE_2D));//为当前绑定的纹理自动生成所有需要的多级渐远纹理
        }
        else {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);//绑定到纹理之后释放图像的内存

        GLuint ibo;
        GLCall(glGenBuffers(1, &ibo));//生成索引缓冲区
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));//绑定索引缓冲区
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indics, indics, GL_STATIC_DRAW));//设置索引缓冲区数据

        Shader shader("res/shaders/Basic.shader");
        shader.Use();

        //获取u_color的location并进行更改
        GLCall(int location = glGetUniformLocation(shader.GetProgramID(), "u_color"));//查询uniform位置不需要先使用着色器程序
        ASSERT(location != -1);//返回值-1表示没有找到uniform
        GLCall(glUniform4f(location, 1.0f, 1.0f, 1.0f, 1.0f));//但是更改uniform值必须先使用着色器程序

        GLCall(glUniform1i(glGetUniformLocation(shader.GetProgramID(), "u_terture1"), 0));
        GLCall(glUniform1i(glGetUniformLocation(shader.GetProgramID(), "u_terture2"), 1));

        GLCall(glBindVertexArray(0));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
        shader.Unuse();

        //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));//使用线框来绘制三角形
        //GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));//切换回默认模式

        GLCall(glEnable(GL_DEPTH_TEST));//开启深度测试，根据z值可以让距离摄像机更近的不会被更远的覆盖

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

            for (unsigned int i = 0;i < 10;i++) {
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, cubePositions[i]);
                float angel = 20.0f * i;
                model = glm::rotate(model, glm::radians(angel), glm::vec3(1.0f, 0.3f, 0.5f));
                GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(model)));

                //观察矩阵，使物体向移动场景的反方向移动
                glm::mat4 view = glm::mat4(1.0f);
                view = glm::lookAt(
                    cameraPos,//摄像机位置
                    cameraPos + cameraFront,//目标位置
                    cameraUp//上向量
                );

                //投影矩阵，使物体按透视的方法变换到裁剪坐标
                glm::mat4 projection = glm::mat4(1.0f);
                projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

                GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view)));
                GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection)));

                GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));
            }

            /*
            //模型矩阵，使物体沿x轴选择，将物体变换到世界坐标
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

            //观察矩阵，使物体向移动场景的反方向移动
            glm::mat4 view = glm::mat4(1.0f);
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

            //投影矩阵，使物体按透视的方法变换到裁剪坐标
            glm::mat4 projection = glm::mat4(1.0f);
            projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, 100.0f);

            GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "model"), 1, GL_FALSE, glm::value_ptr(model)));
            GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "view"), 1, GL_FALSE, glm::value_ptr(view)));
            GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "projection"), 1, GL_FALSE, glm::value_ptr(projection)));
            */

            /*
            glm::mat4 trans = glm::mat4(1.0f);//对图形的语句处理顺序为从下到上
            //trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));//使图像每个轴都缩放0.5倍
            trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));//使图形z轴随时间旋转
            trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));//使用图像位置右移0.5f，下移0.5f

            GLCall(glUniformMatrix4fv(glGetUniformLocation(shader.GetProgramID(), "transfrom"), 1, GL_FALSE, glm::value_ptr(trans)));
            */

            //绘制图形
            //GLCall(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr));//第一个参数为绘制的模式，第二个参数为绘制顶点的数量，第三个参数为索引的类型，第四个为偏移量（或者传递一个索引数组，但是这是当你不在使用索引缓冲对象的时候）

            //交换前缓冲区和后缓冲区，因为如果使用单缓冲区的话，生成的图像需要一步一步的生成出来，看起来不真实，使用双缓冲区的话，前缓冲区为屏幕上显示的图像，后缓冲区为正在渲染的图像，渲染完成之后将两个缓冲区交换，这样可以消除不真实感。
            glfwSwapBuffers(window);

            //检测有没有触发键盘输入、鼠标移动之类的函数，并调用对应的回调函数。 既轮询和处理事件
            glfwPollEvents();
        }
    }
    glfwTerminate();//释放glfw分配的资源
    return 0;
}