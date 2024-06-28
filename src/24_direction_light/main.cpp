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

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
GLuint loadTexture(const char *path);
std::string Shader::dirName;
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

glm::vec3 cameraPos = glm::vec3(0.0f, -10.0f, 8.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// glm::vec3 boxPosition[] = {
//     glm::vec3(0.0f, 0.0f, 0.0f),
//     glm::vec3(2.0f, 2.0f, -2.0f),
//     glm::vec3(-1.5f, -2.2f, -2.5f),
//     glm::vec3(-3.8f, -2.0f, -2.3f),
//     glm::vec3(2.4f, -0.4f, -3.5f),
//     glm::vec3(-1.7f, 3.0f, -2.5f),
//     glm::vec3(1.3f, -2.0f, -2.5f),
//     glm::vec3(1.5f, 2.0f, -2.5f),
//     glm::vec3(1.5f, 0.2f, -1.5f),
//     glm::vec3(-1.3f, 1.0f, -1.5f)
// };
glm::vec3 boxPosition[] = {
    glm::vec3(-4.0f, 0.0f, 0.0f),
    glm::vec3(-3.0f, 0.0f, 0.0f),
    glm::vec3(-2.0f, 0.0f, 0.0f),
    glm::vec3(-1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(2.0f, 0.0f, 0.0f),
    glm::vec3(3.0f, 0.0f, 0.0f),
    glm::vec3(4.0f, 0.0f, 0.0f),
    glm::vec3(5.0f, 0.0f, 0.0f),
};

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

    // 注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader boxShader("./shader/vertex.glsl", "./shader/boxFragment.glsl");
    Shader lightShader("./shader/vertex.glsl", "./shader/lightFragment.glsl");
    float t = 1.0;
    // BoxGeometry box(0.5 * t, 0.5 * t, 0.5 * t);
    SphereGeometry light(0.1f, 24.0f, 24.0f);

    vector<BoxGeometry> boxes;
    int boxNum = 10;
    for (int i = 0; i < boxNum; ++i)
    {
        BoxGeometry b(0.5 * t, 0.5 * t, 0.5 * t);
        boxes.push_back(b);
    }

    GLuint diffuseMap = loadTexture("./static/texture/container2.png");
    GLuint specularMap = loadTexture("./static/texture/container2_specular.png");
    GLuint specularColorMap = loadTexture("./static/texture/lighting_maps_specular_color.png");
    // GLuint emissionMap = loadTexture("./static/texture/matrix.jpg");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, specularColorMap);

    // glActiveTexture(GL_TEXTURE3);
    // glBindTexture(GL_TEXTURE_2D, emissionMap);
    boxShader.use();
    boxShader.setInt("material0.diffuse", 0);
    boxShader.setInt("material0.specular", 1);
    boxShader.setInt("material0.specularColor", 2);
    // boxShader.setInt("material0.emission", 3);

    glm::mat4 projection(1.0f), view(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 0, 1));

    // lightShader
    // glm::vec3 lightPosition(1.0f, -3.0f, 2.0f);
    glm::vec3 lightPosition(5.0f, 0.0f, 3.0f);
    glm::mat4 lightModel(1.0f);
    lightModel = glm::translate(glm::mat4(1.0f), lightPosition);
    lightShader.use();
    lightShader.setMat4("projection", projection);
    lightShader.setMat4("view", view);
    lightShader.setMat4("model", lightModel);

    boxShader.use();
    boxShader.setVec3("light0.ambient", glm::vec3(0.2, 0.2, 0.2));
    boxShader.setVec3("light0.diffuse", glm::vec3(0.5, 0.5, 0.5));
    boxShader.setVec3("light0.specular", glm::vec3(1.0));
    boxShader.setVec4("light0.direction", glm::vec4(lightPosition, 0.0f));

    Material material;
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 32.0f;
    boxShader.setFloat("material0.shininess", material.shininess);
    // 开始时漏了传相机位置给boxFragment，就导致镜面光在box位于光源和相机之间的情况下，才显示，很奇怪的现象。
    boxShader.setVec3("viewPos", cameraPos);

    boxShader.setMat4("projection", projection);
    boxShader.setMat4("view", view);

    float lightAmbient = 0.2f;
    float lightDiffuse = 0.5f;
    float lightSpecular = 1.0f;
    float materialShininess = 32.0f;

    float x = 5.0f;
    float y = 0.0f;
    float z = 3.0f;
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Light_Controls");
        ImGui::SliderFloat("ambientL", &lightAmbient, 0.0f, 1.0f);
        ImGui::SliderFloat("diffuseL", &lightDiffuse, 0.0f, 1.0f);
        ImGui::SliderFloat("specularL", &lightSpecular, 0.0f, 1.0f);
        ImGui::SliderFloat("shininessM", &materialShininess, 0.0f, 256.0f);
        ImGui::SliderFloat("X", &x, -10.0f, 10.0f);
        ImGui::SliderFloat("Y", &y, -10.0f, 10.0f);
        ImGui::SliderFloat("Z", &z, -10.0f, 10.0f);
        ImGui::End();

        processInput(window);
  
        // 渲染指令
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightPosition = glm::vec3(x, y, z);
        lightShader.use();
        glm::mat4 lightModel = glm::translate(glm::mat4(1.0f), lightPosition);
        lightShader.setMat4("model", lightModel);
        glBindVertexArray(light.vao);
        glDrawElements(GL_TRIANGLES, light.indices.size(), GL_UNSIGNED_INT, 0);

        boxShader.use();
        boxShader.setVec3("light0.ambient", glm::vec3(lightAmbient, lightAmbient, lightAmbient));
        boxShader.setVec3("light0.diffuse", glm::vec3(lightDiffuse, lightDiffuse, lightDiffuse));
        boxShader.setVec3("light0.specular", glm::vec3(lightSpecular, lightSpecular, lightSpecular));
        boxShader.setVec4("light0.direction", glm::vec4(lightPosition, 0.0));
        boxShader.setFloat("material0.shininess", materialShininess);

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

        // 渲染 gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glEnable(GL_DEPTH_TEST);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    light.dispose();
    // box.dispose();
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