// 鼠标移动视角 滚轮缩放视角
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

// Material boxMaterial;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void makeMaterial(vector<Material> &v);
Material getMaterial();
std::string Shader::dirName;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

glm::vec3 cameraPos = glm::vec3(0.0f, -8.0f, 2.0f);
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
vector<Material> boxMaterials;
int index = 0;
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

    makeMaterial(boxMaterials);

    // 创建imgui上下文
    const char *glsl_version = "#version 330";
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染器
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
    BoxGeometry box(0.5 * t, 0.5 * t, 0.5 * t);
    SphereGeometry light(0.1f, 24.0f, 24.0f);

    glm::vec3 lightPosition(0.0f);
    glm::mat4 lightModel(1.0f);

    glm::mat4 boxModel(1.0f);
    glm::vec3 boxPosition(0, 0, 0);
    glm::mat4 tMat = glm::translate(glm::mat4(1.0), boxPosition);
    glm::mat4 rMat = glm::rotate(glm::mat4(1.0f), glm::radians(-8.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    boxModel = tMat * rMat;

    boxShader.use();
    boxShader.setVec3("light0.ambient", glm::vec3(0.2, 0.2, 0.2));
    boxShader.setVec3("light0.diffuse", glm::vec3(0.5, 0.5, 0.5));
    boxShader.setVec3("light0.specular", glm::vec3(1.0));
    boxShader.setVec3("light0.position", lightPosition);

    Material material;
    material.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
    material.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
    material.specular = glm::vec3(0.5f, 0.5f, 0.5f);
    material.shininess = 256.0f;
    boxShader.setVec3("material0.ambient", material.ambient);
    boxShader.setVec3("material0.diffuse", material.diffuse);
    boxShader.setVec3("material0.specular", material.specular);
    boxShader.setFloat("material0.shininess", material.shininess);
    // 开始时漏了传相机位置给boxFragment，就导致镜面光在box位于光源和相机之间的情况下，才显示，很奇怪的现象。
    boxShader.setVec3("viewPos", cameraPos);

    glm::mat4 projection(1.0f), view(1.0f);
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    view = glm::lookAt(cameraPos, glm::vec3(0.0f), glm::vec3(0, 0, 1));

    float radius = 2.0f;
    ImVec2 buttonSize = ImVec2(80, 20);
    glm::mat4 lastLightModel(1.0f);
    glm::vec3 lastLightPos(0.0f);
    bool pause = false;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 渲染指令
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // 可以通过imgui.ini设置尺寸位置
        // ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_FirstUseEver);
        // ImGui::SetNextWindowSizeConstraints(ImVec2(100, 100), ImVec2(100, 100));
        ImGui::Begin("imgui");

        if (ImGui::Button("BoxColor", buttonSize))
        {
            material = getMaterial();
        }
        if (ImGui::Button("pause", buttonSize))
        {
            pause = !pause;
        }
        ImGui::End();

        if (pause)
        {
            lightPosition = lastLightPos;
        }
        else
        {
            float time = glfwGetTime();
            lightPosition = glm::vec3(-sin(time) * radius, cos(time) * radius, 0);
        }
        lightModel = glm::translate(glm::mat4(1.0f), lightPosition);
        lastLightPos = lightPosition;
        glm::vec3 lastLightPos;
        lightShader.use();
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);
        lightShader.setMat4("model", lightModel);
        glBindVertexArray(light.vao);
        glDrawElements(GL_TRIANGLES, light.indices.size(), GL_UNSIGNED_INT, 0);

        boxShader.use();
        boxShader.setMat4("projection", projection);
        boxShader.setMat4("view", view);
        boxShader.setMat4("model", boxModel);
        boxShader.setVec3("light0.position", lightPosition);
        boxShader.setVec3("material0.ambient", material.ambient);
        boxShader.setVec3("material0.diffuse", material.diffuse);
        boxShader.setVec3("material0.specular", material.specular);
        boxShader.setFloat("material0.shininess", material.shininess);
        glBindVertexArray(box.vao);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        // 渲染 gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

Material getMaterial()
{
    if(index == 10)
        index = 0;
    cout<<index;
    return boxMaterials[index++];
}

void makeMaterial(vector<Material> &v)
{
    Material emerald;
    emerald.ambient = glm::vec3(0.0215, 0.1745, 0.0215);
    emerald.diffuse = glm::vec3(0.07568, 0.61424, 0.07568);
    emerald.specular = glm::vec3(0.633, 0.727811, 0.633);
    emerald.shininess = 0.6 * 128.0;

    Material jade;
    jade.ambient = glm::vec3(0.135, 0.2225, 0.1575);
    jade.diffuse = glm::vec3(0.54, 0.89, 0.63);
    jade.specular = glm::vec3(0.316228, 0.31622, 0.316228);
    jade.shininess = 0.1 * 128.0;

    Material obsidian;
    obsidian.ambient = glm::vec3(0.05375, 0.05, 0.06625);
    obsidian.diffuse = glm::vec3(0.18275, 0.17, 0.22525);
    obsidian.specular = glm::vec3(0.332741, 0.328634, 0.346435);
    obsidian.shininess = 0.3 * 128.0;

    Material pearl;
    pearl.ambient = glm::vec3(0.25, 0.20725, 0.20725);
    pearl.diffuse = glm::vec3(1, 0.829, 0.829);
    pearl.specular = glm::vec3(0.296648, 0.296648, 0.296648);
    pearl.shininess = 0.088 * 128.0;

    Material ruby;
    ruby.ambient = glm::vec3(0.1745, 0.01175, 0.01175);
    ruby.diffuse = glm::vec3(0.61424, 0.04136, 0.04136);
    ruby.specular = glm::vec3(0.727811, 0.626959, 0.626959);
    ruby.shininess = 0.6 * 128.0;

    Material turquoise;
    turquoise.ambient = glm::vec3(0.1, 0.18725, 0.1745);
    turquoise.diffuse = glm::vec3(0.396, 0.74151, 0.69102);
    turquoise.specular = glm::vec3(0.297254, 0.30829, 0.306678);
    turquoise.shininess = 0.1 * 128.0;

    Material brass;
    brass.ambient = glm::vec3(0.329412, 0.223529, 0.027451);
    brass.diffuse = glm::vec3(0.780392, 0.568627, 0.113725);
    brass.specular = glm::vec3(0.992157, 0.941176, 0.807843);
    brass.shininess = 0.21794872 * 128.0;

    Material bronze;
    bronze.ambient = glm::vec3(0.2125, 0.1275, 0.054);
    bronze.diffuse = glm::vec3(0.714, 0.4284, 0.18144);
    bronze.specular = glm::vec3(0.393548, 0.271906, 0.166721);
    bronze.shininess = 0.2 * 128.0;

    Material chrome;
    chrome.ambient = glm::vec3(0.25, 0.25, 0.25);
    chrome.diffuse = glm::vec3(0.4, 0.4, 0.4);
    chrome.specular = glm::vec3(0.774597, 0.774597, 0.774597);
    chrome.shininess = 0.6 * 128.0;

    Material copper;
    copper.ambient = glm::vec3(0.19125, 0.0735, 0.0225);
    copper.diffuse = glm::vec3(0.7038, 0.27048, 0.0828);
    copper.specular = glm::vec3(0.256777, 0.137622, 0.086014);
    copper.shininess = 0.1 * 128.0;

    v.push_back(emerald);
    v.push_back(jade);
    v.push_back(obsidian);
    v.push_back(pearl);
    v.push_back(ruby);
    v.push_back(turquoise);
    v.push_back(brass);
    v.push_back(bronze);
    v.push_back(chrome);
    v.push_back(copper);
}
