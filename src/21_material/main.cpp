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
#include <geometry/SphereGeometry.h>

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
std::string Shader::dirName;
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

glm::vec3 cameraPos = glm::vec3(0.0f, -8.0f, 3.0f);
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

    // 注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader boxShader("./shader/vertex.glsl", "./shader/boxFragment.glsl");
    Shader lightShader("./shader/vertex.glsl", "./shader/lightFragment.glsl");
    float t = 2.5;
    BoxGeometry box(0.5 * t, 0.5 * t, 0.5 * t);
    // BoxGeometry light(0.2, 0.2, 0.2);
    SphereGeometry light(0.2f,16,16);

    glm::vec3 lightPosition(2,0,2);
    glm::mat4 lightModel(1.0f);
    lightModel = glm::translate(lightModel, lightPosition);

    glm::mat4 boxModel(1.0f);
    glm::vec3 boxPosition(0, -2, 0);
    glm::mat4 tMat = glm::translate(glm::mat4(1.0), boxPosition);
    glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), glm::radians(-30.0f),glm::vec3(0,0,1));
    boxModel = tMat * rMat;

    boxShader.use();
    boxShader.setVec3("light0.ambient", glm::vec3(0.2, 0.2, 0.2));
    boxShader.setVec3("light0.diffuse", glm::vec3(0.5, 0.5, 0.5));
    boxShader.setVec3("light0.specular", glm::vec3(1.0));
    boxShader.setVec3("light0.position", lightPosition);

    boxShader.setVec3("material0.ambient", glm::vec3(1.0f,0.5f,0.31f));
    boxShader.setVec3("material0.diffuse", glm::vec3(1.0f,0.5f,0.31f));
    boxShader.setVec3("material0.specular",glm::vec3(0.5f,0.5f,0.5f));
    boxShader.setFloat("material0.shininess", 32.0f);


    glm::mat4 projection(1.0f), view(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 0, 1));


    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 渲染指令
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        boxShader.use();
        boxShader.setMat4("projection", projection);
        boxShader.setMat4("view", view);
        boxShader.setMat4("model", boxModel);
        glBindVertexArray(box.vao);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("model", lightModel);
        glBindVertexArray(light.vao);
        glDrawElements(GL_TRIANGLES, light.indices.size(), GL_UNSIGNED_INT, 0);

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

    // if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    // {
    //     pause = !pause;
    // }

    // camera.ProcessKeyboard(deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}