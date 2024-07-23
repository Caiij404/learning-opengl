#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <map>

#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <tool/shader.h>
#include <tool/camera.h>
#include <tool/gui.h>
#include <tool/model.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap);

std::string Shader::dirName;

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(glm::vec3(0.0, 1.0, 6.0));
#include <tool/mySpace.h>
using namespace std;
using namespace mySpace;
bool stopPainting = false;

float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    GLFWwindow *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL");

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

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader explodeShader("./shader/explode_vert.glsl", "./shader/explode_frag.glsl", "./shader/explode_geom.glsl");
    Shader skyBoxShader("./shader/cube_map_vert.glsl", "./shader/cube_map_frag.glsl");
    Shader normalShader("./shader/normal_vert.glsl", "./shader/normal_frag.glsl", "./shader/normal_geom.glsl");
    BoxGeometry skyBox(1.0f, 1.0f, 1.0f); // 天空盒

    // 立方体贴图
    vector<const char *> textures_faces;
    textures_faces.push_back("./static/texture/skybox/right.jpg");
    textures_faces.push_back("./static/texture/skybox/left.jpg");
    textures_faces.push_back("./static/texture/skybox/top.jpg");
    textures_faces.push_back("./static/texture/skybox/bottom.jpg");
    textures_faces.push_back("./static/texture/skybox/back.jpg");
    textures_faces.push_back("./static/texture/skybox/front.jpg");

    GLuint skyBoxTextrue = loadCubeTexture(textures_faces);
    Model ourModel("./static/model/nanosuit/nanosuit.obj");

    glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    glm::mat4 model(1.0f);
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glEnable(GL_DEPTH_TEST);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;
 
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // // 一开始很sb的把drawSkyBox方法放在glClear前面。。。结果就是怎么都画不出天空盒
        drawSkyBox(skyBoxShader, skyBox, skyBoxTextrue);

        float time = glfwGetTime();
        glm::mat4 view = camera.GetViewMatrix();
        model = glm::rotate(glm::mat4(1.0f), glm::radians(15.0f * (float)time), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.13f, 0.13f, 0.13f));
        explodeShader.use();
        explodeShader.setMat4("projection", projection);
        explodeShader.setMat4("view", view);
        explodeShader.setMat4("model", model);
        explodeShader.setFloat("time", time);
        ourModel.Draw(explodeShader);

        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);
        normalShader.setMat4("model", model);
        ourModel.Draw(normalShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    skyBox.dispose();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    Camera_KeyBoardAction action;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        action = FORWARD;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        action = BACKWARD;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        action = LEFT;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        action = RIGHT;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        action = UP;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        action = DOWN;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        action = ROTATEL;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        action = ROTATER;
    camera.ProcessKeyboard(action, deltaTime);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset, TRANSLATION);
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset, ROTATION);
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap)
{
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_DEPTH_TEST);

    shader.use();
    glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // 移除平移分量
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
    glBindVertexArray(geometry.vao);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
}