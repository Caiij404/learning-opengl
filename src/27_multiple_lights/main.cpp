// 光照贴图
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <tool/shader.h>
#include <geometry/BoxGeometry.h>
#include <geometry/SphereGeometry.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>
#include <tool/glm_io.hpp>
#include <tool/camera.h>
#include <tool/gui.h>

using namespace std;

struct Material
{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

struct DirLight
{
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight
{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

#define POINT_LIGHT_NUM 4

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outerCutoff;
};

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
GLuint loadTexture(const char *path);
std::string Shader::dirName;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

glm::vec3 cameraPos = glm::vec3(0.0f, -2.0f, 4.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 boxPosition[] = {
    glm::vec3(-1.0, 1.0, 0.0),
    glm::vec3(-1.0, 0.0, 0.0),
    glm::vec3(-1.0, -1.0, 0.0),

    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 0.0, 0.0),
    glm::vec3(0.0, -1.0, 0.0),

    glm::vec3(1.0, 1.0, 0.0),
    glm::vec3(1.0, 0.0, 0.0),
    glm::vec3(1.0, -1.0, 0.0),
};

float lastX = (float)SCR_WIDTH / 2.0f;
float lastY = (float)SCR_HEIGHT / 2.0f;
Camera camera(cameraPos, cameraUp);
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

    // 注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    const char *glsl_version = "#version 330";
    // 创建imgui上下文
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // 设置视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Shader boxShader("./shader/vertex.glsl", "./shader/boxFragment.glsl");
    Shader lightShader("./shader/vertex.glsl", "./shader/lightFragment.glsl");
    float t = 1.0;
    SphereGeometry dLightGeo(0.1f, 24.0f, 24.0f);
    SphereGeometry pLightGeo[POINT_LIGHT_NUM];
    for (int i = 0; i < POINT_LIGHT_NUM; ++i)
    {
        pLightGeo[i] = SphereGeometry(0.1f, 24.0f, 24.0f);
    }

    vector<BoxGeometry> boxes;
    int boxNum = 9;
    for (int i = 0; i < boxNum; ++i)
    {
        BoxGeometry b(0.5 * t, 0.5 * t, 0.5 * t);
        boxes.push_back(b);
    }

    GLuint diffuseMap = loadTexture("./static/texture/container2.png");
    GLuint specularMap = loadTexture("./static/texture/container2_specular.png");
    GLuint spotLightMap = loadTexture("./static/texture/awesomeface.png");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, spotLightMap);

    DirLight dLight;
    dLight.direction = glm::vec3(1.0f, 1.0f, 1.0f);
    dLight.ambient = glm::vec3(0.01f);
    dLight.diffuse = glm::vec3(0.2f);
    dLight.specular = glm::vec3(1.0f); 

    PointLight pLights[POINT_LIGHT_NUM];
    for (int i = 0; i < POINT_LIGHT_NUM; ++i)
    {
        pLights[i].ambient = glm::vec3(0.01f);
        pLights[i].diffuse = glm::vec3(0.2f);
        pLights[i].specular = glm::vec3(1.0f);
        pLights[i].constant = 1.0f;
        pLights[i].linear = 0.09f;
        pLights[i].quadratic = 0.032f;
        glm::vec3 pos(0.0f);
        switch (i)
        {
        case 0:
            pos = glm::vec3(2.0f, -3.0f, 2.0f);
            break;
        case 1:
            pos = glm::vec3(2.0f, 3.0f, -4.0f);
            break;
        case 2:
            pos = glm::vec3(-2.0f, 3.0f, -2.0f);
            break;
        case 3:
            pos = glm::vec3(-2.0f, -3.0f, 4.0f);
            break;
        }
        pLights[i].position = pos;
    }

    SpotLight sLight;
    sLight.ambient = glm::vec3(0.0f);
    sLight.diffuse = glm::vec3(1.0f);
    sLight.specular = glm::vec3(1.0f);
    sLight.constant = 1.0f;
    sLight.linear = 0.09f;
    sLight.quadratic = 0.032f;
    sLight.cutoff = cos(glm::radians(12.5f));
    sLight.outerCutoff = cos(glm::radians(17.5f));
    sLight.position = camera.Position;
    sLight.direction = camera.Front;

    boxShader.use();
    boxShader.setInt("material0.diffuse", 0);
    boxShader.setInt("material0.specular", 1);
    boxShader.setInt("material0.spotLightMap", 2);

    boxShader.setVec3("dLight.direction", dLight.direction);
    boxShader.setVec3("dLight.ambient", dLight.ambient);
    boxShader.setVec3("dLight.diffuse", dLight.diffuse);
    boxShader.setVec3("dLight.specular", dLight.specular);

    for (int i = 0; i < POINT_LIGHT_NUM; ++i)
    {
        boxShader.setVec3("pLight[" + std::to_string(i) + "].ambient", pLights[i].ambient);
        boxShader.setVec3("pLight[" + std::to_string(i) + "].diffuse", pLights[i].diffuse);
        boxShader.setVec3("pLight[" + std::to_string(i) + "].specular", pLights[i].specular);
        boxShader.setVec3("pLight[" + std::to_string(i) + "].position", pLights[i].position);
        boxShader.setFloat("pLight[" + std::to_string(i) + "].constant", pLights[i].constant);
        boxShader.setFloat("pLight[" + std::to_string(i) + "].linear", pLights[i].linear);
        boxShader.setFloat("pLight[" + std::to_string(i) + "].quadratic", pLights[i].quadratic);
    }

    boxShader.setVec3("sLight.ambient", sLight.ambient);
    boxShader.setVec3("sLight.diffuse", sLight.diffuse);
    boxShader.setVec3("sLight.specular", sLight.specular);
    boxShader.setVec3("sLight.position", sLight.position);
    boxShader.setVec3("sLight.direction", sLight.direction);
    boxShader.setFloat("sLight.constant", sLight.constant);
    boxShader.setFloat("sLight.linear", sLight.linear);
    boxShader.setFloat("sLight.quadratic", sLight.quadratic);
    boxShader.setFloat("sLight.cutoff", sLight.cutoff);
    boxShader.setFloat("sLight.outerCutoff", sLight.outerCutoff);

    glm::mat4 projection(1.0f), view(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = camera.GetViewMatrix();

    Material material;
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 32.0f;
    boxShader.setFloat("material0.shininess", material.shininess);
    // 开始时漏了传相机位置给boxFragment，就导致镜面光在box位于光源和相机之间的情况下，才显示，很奇怪的现象。
    boxShader.setVec3("viewPos", camera.Position);
    boxShader.setMat4("projection", projection);
    boxShader.setMat4("view", view);

    // lightShader
    glm::vec3 dLightPosition(5.0f, 0.0f, 3.0f);
    glm::mat4 lightModel(1.0f);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // 渲染指令
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        // draw light geo
        // direction light
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightModel = glm::translate(glm::mat4(1.0f), dLightPosition);
        lightShader.setMat4("model", lightModel);
        glBindVertexArray(dLightGeo.vao);
        glDrawElements(GL_TRIANGLES, dLightGeo.indices.size(), GL_UNSIGNED_INT, 0);

        // point lights
        for (int i = 0; i < POINT_LIGHT_NUM; ++i)
        {
            lightModel = glm::translate(glm::mat4(1.0f), pLights[i].position);
            lightShader.setMat4("model", lightModel);
            glBindVertexArray(pLightGeo[i].vao);
            glDrawElements(GL_TRIANGLES, pLightGeo[i].indices.size(), GL_UNSIGNED_INT, 0);
        }

        boxShader.use();
        boxShader.setMat4("view", view);
        boxShader.setMat4("projection", projection);
        boxShader.setVec3("viewPos", camera.Position);
        boxShader.setVec3("sLight.direction", camera.Front);
        boxShader.setVec3("sLight.position", camera.Position);

        for (int i = 0; i < boxNum; ++i)
        {
            BoxGeometry box = boxes[i];
            glm::mat4 tMat = glm::translate(glm::mat4(1.0f), boxPosition[i]);
            glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), glm::radians(20.0f * i), glm::normalize(glm::vec3(i, i + 1, (i + 1) * 4)));
            // glm::mat4 boxModel = tMat * rMat;
            glm::mat4 boxModel = tMat;
            boxShader.setMat4("model", boxModel);

            glBindVertexArray(box.vao);
            glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);
        }

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    dLightGeo.dispose();
    for (int i = 0; i < POINT_LIGHT_NUM; ++i)
    {
        pLightGeo[i].dispose();
    }
    for (int i = 0; i < boxNum; ++i)
    {
        boxes[i].dispose();
    }
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

unsigned int loadTexture(const char *path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    stbi_set_flip_vertically_on_load(true);

    // 加载图像
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);

    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        // 通过glTexImage2D生成纹理，将纹理图像附加在纹理对象texture上
        // 纹理目标(Target) 多级渐远纹理级别 纹理存储形式(我们图像只有rgb信息) 图像宽 图像高 总是0 源图的格式 源图数据类型 图像数据
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        // 设置多级渐远纹理
        glGenerateMipmap(GL_TEXTURE_2D);

        // 设置环绕和过滤方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}