#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>
#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <tool/shader.h>
#include <tool/camera.h>
#include <tool/gui.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);

std::string Shader::dirName;
int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0, 1.0, 6.0));
#include <tool/mySpace.h>
using namespace std;
using namespace mySpace;

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

    Shader sceneShader("./shader/sceneVert.glsl", "./shader/sceneFrag.glsl");
    Shader lightShader("./shader/lightVert.glsl", "./shader/lightFrag.glsl");

    PlaneGeometry planeGeometry(10.0f, 10.0f, 10.0f, 10.0f);
    BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);
    SphereGeometry sphereGeometry(0.1f, 10.0f, 10.0f);

    GLuint brick = loadTexture("./static/texture/brick_diffuse.jpg");
    GLuint board = loadTexture("./static/texture/wall.jpg");

    glm::vec3 lightPosition = glm::vec3(1.0, 2.5, 2.0); // 光照位置

    // 点光源的位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 1.0f, 1.5f),
        glm::vec3(2.3f, 3.0f, -4.0f),
        glm::vec3(-4.0f, 2.0f, 1.0f),
        glm::vec3(1.4f, 2.0f, 0.3f)};

    // 点光源颜色
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)};

    float myNear = 0.1;
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("tune near");
        ImGui::SliderFloat("Near", &myNear, 0.1f, 5.0f);
        ImGui::End();
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float radius = 5.0f;
        float camX = sin(currentFrame * 0.5) * radius;
        float camZ = cos(currentFrame * 0.5) * radius;
        pointLightPositions[0].z = camZ;
        pointLightPositions[0].x = camX;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)(SCREEN_WIDTH / SCREEN_HEIGHT), myNear, 100.0f);

        sceneShader.use();
        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);
        sceneShader.setVec3("viewPos", camera.Position);

        for (int i = 0; i < 4; ++i)
        {
            // 设置点光源属性
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.01f, 0.01f, 0.01f);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            sceneShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            // // 设置衰减
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
            sceneShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }

        glm::mat4 model(1.0f);
        model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));

        sceneShader.setFloat("uvScale", 4.0);
        sceneShader.setMat4("model", model);

        // 绘制地面
        glBindTexture(GL_TEXTURE_2D, board);
        glBindVertexArray(planeGeometry.vao);
        glDrawElements(GL_TRIANGLES, planeGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 绘制砖块
        glBindTexture(GL_TEXTURE_2D, brick);

        model = glm::scale(glm::mat4(1.0f), glm::vec3(2.0));
        model = glm::translate(model, glm::vec3(1.0, 0.5, -1.0));
        sceneShader.setFloat("uvScale", 1.0f);
        sceneShader.setMat4("model", model);
        glBindVertexArray(boxGeometry.vao);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0.5, 2.0));
        sceneShader.setMat4("model", model);
        glBindVertexArray(boxGeometry.vao);
        glDrawElements(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 绘制灯光物体
        // ***********************************
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);

        glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(currentFrame) * 2.0, lightPosition.y, lightPosition.z);
        model = glm::translate(glm::mat4(1.0f), lightPos);
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindVertexArray(sphereGeometry.vao);
        glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        for (int i = 0; i < 4; ++i)
        {
            model = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", pointLightColors[i]);

            glBindVertexArray(sphereGeometry.vao);
            glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        }

        // 渲染 gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    planeGeometry.dispose();
    boxGeometry.dispose();
    sphereGeometry.dispose();
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        action = SPACEBAR;
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