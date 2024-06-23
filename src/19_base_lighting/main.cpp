// 鼠标移动视角 滚轮缩放视角
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <tool/shader.h>

#include <geometry/BoxGeometry.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/glm_io.hpp>
#include <tool/camera.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
std::string Shader::dirName;
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

glm::vec3 cameraPos = glm::vec3(0.0f, -4.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;
float lastX = (float)SCR_WIDTH / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;
float fov = 45.0f;
float sensitivity = 0.1;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

bool pause = false;

int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    glfwInit();
    // 设置主要和次要版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口对象
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    // 设置视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);

    camera.window = window;

    // 注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 告诉GLFW捕捉鼠标
    // tell GLFW to capture our mouse
    // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader boxShader("./shader/vertex.glsl", "./shader/boxFragment.glsl");
    Shader lightShader("./shader/vertex.glsl", "./shader/lightFragment.glsl");
    float t = 2.5;
    BoxGeometry box(0.5 * t, 0.5 * t, 0.5 * t);
    BoxGeometry light(0.2, 0.2, 0.2);

    {
        // ______生成纹理______

        // 纹理对象
        // 参数：纹理个数、存放ID的数组
        GLuint textures[2];
        glGenTextures(2, textures);
        // 绑定纹理前先激活纹理单元Texture Unit（纹理的位置值）。一个纹理的默认纹理单元是0，0号默认激活
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);

        // 设置环绕和过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // flip the image vertically, so the first pixel in the output array is the bottom left
        // 图像y轴翻转，使得输出数组的第一个像素位于左下角
        stbi_set_flip_vertically_on_load(true);

        // 加载图像
        int width, height, nrChannels;
        // stbi_load会将图像的宽度、高度和颜色通道个数填充到这三个变量中，后续生成纹理时会用到
        unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);

        if (data)
        {
            // 通过glTexImage2D生成纹理，将纹理图像附加在纹理对象texture上
            // 纹理目标(Target) 多级渐远纹理级别 纹理存储形式(我们图像只有rgb信息) 图像宽 图像高 总是0 源图的格式 源图数据类型 图像数据
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

            // 设置多级渐远纹理
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }
        stbi_image_free(data);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        // 设置环绕和过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 第二张图
        data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);
        if (data)
        {
            // !!!!!! png是四通道图，GL_RGBA
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        stbi_image_free(data);

        // glUniform1i 是数字1，不是字母l
        // 纹理单元数量超过1个，需要glUniform1i来设置每个采样器的方式告诉OpenGL每个着色器属于哪个纹理单元
        // boxShader.use(); // 不要忘记在设置uniform变量之前激活着色器程序！
        // boxShader.setInt("texture1", 0);
        // boxShader.setInt("texture2", 1);
    }
    glm::vec3 lightPosition(0.5f, 2.0f, -1.0f);
    glm::mat4 lightModel(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);
    lightShader.use();
    // 是box的片段着色器要这个光源的位置信息
    // lightShader.setVec3("lightPos", lightPosition);

    boxShader.use();
    boxShader.setVec3("lightColor", glm::vec3(1.0f));
    boxShader.setVec3("objColor", glm::vec3(1.0f, 0.5f, 0.31f));
    boxShader.setVec3("lightPos", lightPosition);
    boxShader.setVec3("viewPos", camera.Position);

    glm::mat4 projection(1.0f), view(1.0f);
    glm::mat4 lastBoxModel(1.0f);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        processInput(window);

        // 渲染指令
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 前面忘了给camera.Zoom加上radians方法，导致滚轮缩放异常
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)(SCR_HEIGHT), 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        // view = glm::lookAt(cameraPos, glm::vec3(0,0,0), cameraUp);

        glm::mat4 boxModel(1.0f);
        glm::qua<float> qu = glm::qua<float>(glm::vec3(glfwGetTime(), glfwGetTime(), glfwGetTime()));
        if (pause)
        {
            boxModel = lastBoxModel;
        }
        else
        {
            boxModel = glm::mat4_cast(qu);
            lastBoxModel = boxModel;
        }
        boxShader.use();
        boxShader.setMat4("projection", projection);
        boxShader.setMat4("view", view);
        boxShader.setMat4("model", boxModel);
        glBindVertexArray(box.vao);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        glm::mat4 mat2 = projection * view * lightModel;
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("model", lightModel);
        glBindVertexArray(light.vao);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    box.dispose();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        pause = !pause;
    }

    camera.ProcessKeyboard(deltaTime);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}