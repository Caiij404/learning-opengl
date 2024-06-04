// 着色器
// 更多属性 location
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "Utils/utils.h"

// 顶点数组对象: Vertex Array Object, VAO
// 顶点缓冲对象: Vertex Buffer Object, VBO
// 元素缓冲对象: Element Buffer Object, EBO 或 索引缓冲对象: Index Buffer Object, IBO
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // std::cout<<vertexShaderSource<<std::endl;

    // 顶点着色器
    // 为了能够让OpenGL使用，必须运行时动态编译它的源码。
    // 创建着色器对象，还是用ID来引用
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 把源码附加到着色器对象上，然后编译。2.传递源码字符串的数量；3.源码
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(vertexShader, 1, &vs032, NULL);
    glCompileShader(vertexShader);
    // 如果希望知道glCompileShader是否编译成功，有没有报错
    compileInfoLog(vertexShader, logType::SHADER);

    // 片段着色器
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fs032, NULL);
    glCompileShader(fragmentShader);
    compileInfoLog(fragmentShader, logType::SHADER);

    // 着色器程序
    // 两个着色器编译完成后，需要把二者对象链接到一个用来渲染的着色器程序Shader Program
    // 着色器程序对象(Shader Program Object)是多个着色器合并之后最终链接完成的版本。
    // 如果要使用刚编译的着色器，就必须把它们链接Link为一个着色器程序，然后在渲染对象时激活。
    // 当链接着色器至一个程序时，它会把每个着色器的输出链接到下一个着色器的输入。当不匹配时，会有链接错误。
    unsigned int shaderProgram;
    // glCreateProgram函数创建一个程序，并返回新创建程序对象的ID引用
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    compileInfoLog(shaderProgram, logType::SHADER_PROGRAM);

    // 在程序对象链接着色器后，可以删除着色器对象了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  // bottom left
         0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f   // top 
    };

    unsigned int vbo, vao;
    glGenVertexArrays(1, & vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    glUseProgram(shaderProgram);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}


/* // 着色器
// Uniform
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <iostream>

#include "Utils/utils.h"

// 顶点数组对象: Vertex Array Object, VAO
// 顶点缓冲对象: Vertex Buffer Object, VBO
// 元素缓冲对象: Element Buffer Object, EBO 或 索引缓冲对象: Index Buffer Object, IBO
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // std::cout<<vertexShaderSource<<std::endl;

    // 顶点着色器
    // 为了能够让OpenGL使用，必须运行时动态编译它的源码。
    // 创建着色器对象，还是用ID来引用
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // 把源码附加到着色器对象上，然后编译。2.传递源码字符串的数量；3.源码
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(vertexShader, 1, &vs03, NULL);
    glCompileShader(vertexShader);
    // 如果希望知道glCompileShader是否编译成功，有没有报错
    compileInfoLog(vertexShader, logType::SHADER);

    // 片段着色器
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fs031, NULL);
    glCompileShader(fragmentShader);
    compileInfoLog(fragmentShader, logType::SHADER);

    // 着色器程序
    // 两个着色器编译完成后，需要把二者对象链接到一个用来渲染的着色器程序Shader Program
    // 着色器程序对象(Shader Program Object)是多个着色器合并之后最终链接完成的版本。
    // 如果要使用刚编译的着色器，就必须把它们链接Link为一个着色器程序，然后在渲染对象时激活。
    // 当链接着色器至一个程序时，它会把每个着色器的输出链接到下一个着色器的输入。当不匹配时，会有链接错误。
    unsigned int shaderProgram;
    // glCreateProgram函数创建一个程序，并返回新创建程序对象的ID引用
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    compileInfoLog(shaderProgram, logType::SHADER_PROGRAM);

    // 在程序对象链接着色器后，可以删除着色器对象了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 激活程序对象
    // glUseProgram(shaderProgram);

    // 在OpenGL中绘制一个物体，代码会像是这样：
    // 0. 复制顶点数组到缓冲中供OpenGL使用
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    // // 1. 设置顶点属性指针
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);
    // // 2. 当我们渲染一个物体时要使用着色器程序
    // glUseProgram(shaderProgram);
    // // 3. 绘制物体
    // someOpenGLFunctionThatDrawsOurTriangle();

    // 每绘制一个物体，都需要重复上述过程，这导致程序变得繁琐起来

    // --------------尝试画一个三角形--------------
    // float vertices1[] = {
    //     -0.5f, -0.5f, 0.0f,
    //     0.5f, -0.5f, 0.0f,
    //     0.0f, 0.5f, 0.0f};

    // 定义好顶点数据后，发送给图形渲染管线的第一个处理阶段：顶点着色器。
    // 它会在GPU上创建内存用于存储顶点数据，还要配置OpenGL如何解释这些内存，并指定其如何发送给显卡。接着就是处理内存中的顶点数据
    // 顶点缓冲对象vbo管理这个内存，他会在GPU内存中存储大量顶点。使用vbo的好处就是可以一次性发送大批数据到显卡上，让顶点着色器能立即访问顶点。
    // 生成带有缓冲ID的vbo对象
    // unsigned int vbo;
    // glGenBuffers(1, &vbo);
    // 把vbo绑定到顶点缓冲对象的缓冲类型GL_ARRAY_BUFFER上。后面使用的（GL_ARRAY_BUFFER目标上的）缓冲调用都会用当前绑定的缓冲vbo
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // 然后通过glBufferData，就能把顶点数据复制到缓冲对象vbo的内存中
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    // 顶点数组对象 VAO
    // unsigned int vao, vbo;
    // glGenVertexArrays(1, &vao);
    // glGenBuffers(1, &vbo);

    // glBindVertexArray(vao);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    // 到这一步，我们已经把输入的顶点数据发送给了GPU，并指示GPU如何在顶点和片段着色器中处理它。
    // 但还需要让它知道如何解释内存中的顶点数据，以及如何将顶点数据链接到顶点着色器的属性上。

    // 链接顶点属性
    // 告诉OpenGL如何解析顶点数据
    // 1.location == 0；2.顶点属性大小 vec3；3.数据类型； 4.是否标准化（true则所有数据会被映射到0~1之间，有符号则是-1 ~ 1）；5.步长，连续顶点属性组的间隔；6.偏移量
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // glEnableVertexAttribArray(0);

    // 解绑
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // --------------尝试画一个三角形--------------

    // --------------通过画两个三角形来生成一个矩形--------------
    float vertices2[] = {
        0.5f,
        0.5f,
        0.0f,
        0.5f,
        -0.5f,
        0.0f,
        -0.5f,
        -0.5f,
        0.0f,
        -0.5f,
        0.5f,
        0.0f,
    };
    unsigned int indices[] = {
        // 第一个三角形
        0, 1, 3,
        // 第二个三角形
        1, 2, 3};

    unsigned int vbo, vao, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    // 给vbo unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // vao还在使用时，ebo不能unbind
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // --------------通过画两个三角形来生成一个矩形--------------

    // 线框模式
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 普通模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2, 0.1, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // 使用着色器程序
        glUseProgram(shaderProgram);

        // 使用glfwGetTime获取运行秒数，然后用sin函数让颜色在0.0 ~ 1.0之间变化
        float timeValue = glfwGetTime();
        float r = (sin(timeValue) / 2) + 0.5;
        float g = (cos(timeValue) / 2) + 0.5;
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, r, g, 1.0, 1.0);

        // 画一个三角形
        // glDrawArrays(GL_TRIANGLES, 0, 3);

        // 画两个三角形
        // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // 交换缓冲并查询IO事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glDeleteVertexArrays(1, &vao);
    // glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
} */