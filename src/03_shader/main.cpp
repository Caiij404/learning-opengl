// 你好，窗口
#include <random>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// 定义函数
void frameBuffer_Size_CB(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
    std::cout << "w: " << w << "   " << "h: " << h << std::endl;
}

// 声明函数
void keyDown(GLFWwindow *window);

int main()
{
    glfwInit();
    // 主版本号3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    // 次版本号3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 核心模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, frameBuffer_Size_CB);

    int n;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n);
    std::cout << "----------------Maximum n of vertex attributes supported: " << n << "----------------" << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        // 检查按键输入
        keyDown(window);

        // 检查并调用事件，交换缓冲 （没有则会崩溃）
        glfwPollEvents();
        glfwSwapBuffers(window);

        // 清空屏幕颜色缓存，不然只会显示黑色
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.2f, 0.3, 1.0f);
    }

    glfwTerminate();
    return 0;
}

void keyDown(GLFWwindow *window)
{
    // 绑定快捷键，escape退出程序
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}