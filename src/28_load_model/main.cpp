#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

#include <geometry/BoxGeometry.h>
#include <geometry/PlaneGeometry.h>
#include <geometry/SphereGeometry.h>

#include <tool/shader.h>
#include <tool/camera.h>

#define STB_IMAGE_IMPLEMENTATION
#include <tool/stb_image.h>

#include <tool/gui.h>

#include <tool/mesh.h>
#include <tool/model.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);

std::string Shader::dirName;

int SCREEN_WIDTH = 1600;
int SCREEN_HEIGHT = 1200;
// int SCREEN_WIDTH = 1600;
// int SCREEN_HEIGHT = 1200;

// camera value
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0, 0.0, 5.0));
#include <tool/mySpace.h>
using namespace std;
using namespace mySpace;
int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];

    GLFWwindow *window = initWindow(SCREEN_WIDTH,SCREEN_HEIGHT, "LearnOpenGL");

    // -----------------------
    // 创建imgui上下文
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // -----------------------

    // 鼠标键盘事件
    // 1.注册窗口变化监听
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // 2.鼠标事件
    glfwSetCursorPosCallback(window, mouse_callback);

    // Shader ourShader("./shader/vertex_m.glsl", "./shader/fragment_m.glsl");
    Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");
    Shader lightObjectShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl");

    PlaneGeometry planeGeometry(1.0, 1.0, 1.0, 1.0);
    BoxGeometry boxGeometry(1.0, 1.0, 1.0);
    SphereGeometry sphereGeometry(0.1, 10.0, 10.0);

    unsigned int diffuseMap = loadTexture("./static/texture/container2.png");
    unsigned int specularMap = loadTexture("./static/texture/container2_specular.png");
    unsigned int awesomeMap = loadTexture("./static/texture/awesomeface.png");
    ourShader.use();
    // ourShader.setInt("material.diffuse", 0);
    // ourShader.setInt("material.specular", 1);
 
    // ourShader.setInt("awesomeMap", 2);

    float factor = 0.0;
    float fov = 45.0f; // 视锥体的角度
    glm::vec3 view_translate = glm::vec3(0.0, 0.0, -5.0);
    ImVec4 clear_color = ImVec4(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0); // 25, 25, 25

    glm::vec3 lightPosition = glm::vec3(1.0, 2.5, 2.0); // 光照位置

    // 传递材质属性
    ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    ourShader.setFloat("material.shininess", 32.0f);

    // 设置平行光光照属性
    ourShader.setVec3("directionLight.ambient", 0.01f, 0.01f, 0.01f);
    ourShader.setVec3("directionLight.diffuse", 0.2f, 0.2f, 0.2f); // 将光照调暗了一些以搭配场景
    ourShader.setVec3("directionLight.specular", 1.0f, 1.0f, 1.0f);

    // 设置聚光光照属性
    ourShader.setVec3("spotLight.position", glm::vec3(0.0f, 0.0f, 2.0f));
    ourShader.setVec3("spotLight.direction", camera.Front);
    ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    ourShader.setFloat("spotLight.constant", 1.0f);
    ourShader.setFloat("spotLight.linear", 0.09);
    ourShader.setFloat("spotLight.quadratic", 0.032);
    ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

    // 设置衰减
    ourShader.setFloat("light.constant", 1.0f);
    ourShader.setFloat("light.linear", 0.09f);
    ourShader.setFloat("light.quadratic", 0.032f);


    // 点光源的位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 0.2f, 1.5f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f, 2.0f, -12.0f),
        glm::vec3(0.0f, 0.0f, -3.0f)};

    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)};

    Model ourModel("./static/model/nanosuit/nanosuit.obj");
    // Model ourModel("./static/model/cerberus/Cerberus.obj");

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        // 在标题中显示帧率信息
        // *************************************************************************
        int fps_value = (int)round(ImGui::GetIO().Framerate);
        int ms_value = (int)round(1000.0f / ImGui::GetIO().Framerate);

        std::string FPS = std::to_string(fps_value);
        std::string ms = std::to_string(ms_value);
        std::string newTitle = "LearnOpenGL - " + ms + " ms/frame " + FPS;
        glfwSetWindowTitle(window, newTitle.c_str());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui::Begin("controls");
        // ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        // ImGui::End();
        //  *************************************************************************

        // 渲染指令
        // ...
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();

        factor = glfwGetTime();
        ourShader.setFloat("factor", -factor * 0.3);

        // 修改光源颜色
        glm::vec3 lightColor;
        lightColor.x = sin(glfwGetTime() * 2.0f);
        lightColor.y = sin(glfwGetTime() * 0.7f);
        lightColor.z = sin(glfwGetTime() * 1.3f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, awesomeMap);

        float radius = 10.0f;
        float camX = sin(glfwGetTime()) * radius;
        float camZ = cos(glfwGetTime()) * radius;

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

        glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(glfwGetTime()) * 2.0, lightPosition.y, lightPosition.z);

        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        ourShader.setVec3("directionLight.direction", lightPos); // 光源位置
        ourShader.setVec3("viewPos", camera.Position);

        for (unsigned int i = 0; i < 4; i++)
        {
            // 设置点光源属性
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.01f, 0.01f, 0.01f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", pointLightColors[i]);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);

            // // 设置衰减
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
        }

        glm::mat4 model = glm::mat4(1.0f);

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(15.0f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.13f, 0.13f, 0.13f));
        ourShader.setMat4("model", model);

        ourModel.Draw(ourShader);

        // 绘制灯光物体
        lightObjectShader.use();
        lightObjectShader.setMat4("view", view);
        lightObjectShader.setMat4("projection", projection);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);

        lightObjectShader.setMat4("model", model);
        lightObjectShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

        glBindVertexArray(sphereGeometry.vao);
        glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);

            lightObjectShader.setMat4("model", model);
            lightObjectShader.setVec3("lightColor", pointLightColors[i]);

            glBindVertexArray(sphereGeometry.vao);
            glDrawElements(GL_TRIANGLES, sphereGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        }

        // 渲染 gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    boxGeometry.dispose();
    planeGeometry.dispose();
    sphereGeometry.dispose();
    glfwTerminate();

    return 0;
}

// 窗口变动监听
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 键盘输入监听
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

    // 相机按键控制
    // 相机移动
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
}

// 鼠标移动监听
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}