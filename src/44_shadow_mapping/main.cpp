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
#include <tool/myImGui.hpp>
#include <tool/shader.h>
#include <tool/camera.h>
#include <tool/model.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap);

void drawMesh(BufferGeometry &geo);
void drawLightObject(Shader shader, BufferGeometry geometry, glm::vec3 position);

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

    myImGui gui(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glEnable(GL_FRAMEBUFFER_SRGB);

    Shader shadowShader("./shader/shadow_map_vert.glsl", "./shader/shadow_map_frag.glsl");
    Shader finalShadowShader("./shader/shadow_final_vert.glsl", "./shader/shadow_final_frag.glsl");
    Shader lightObjectShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl");
    Shader quadShader("./shader/shadow_quad_vert.glsl", "./shader/shadow_quad_frag.glsl");

    GLuint floorMap = loadTexture("./static/texture/wood.png");
    GLuint brickMap = loadTexture("./static/texture/brick_diffuse.jpg"); // 砖墙

    PlaneGeometry floor(10.0f, 10.0f);
    BoxGeometry brick(1.0f, 1.0f, 1.0f);
    SphereGeometry pointLightGeometry(0.06, 10.0, 10.0); // 点光源位置显示
    PlaneGeometry quadGeometry(6.0, 6.0);                // 测试面板

    // 阴影贴图

    // 1.创建帧缓冲
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // 2.创建2D纹理，提供给帧缓冲中的深度缓冲使用
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // 因为只关心深度值而不关系颜色，因此把纹理格式指定为GL_DEPTH_COMPONENT，还要把纹理宽高设置为1024，这是深度贴图的分辨率
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 3.把生成的纹理作为帧缓冲的深度缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // 因为只关心深度值而不关系颜色，但不包含颜色缓冲的帧缓冲是不完整的，因此需要显示explicitly告诉OpenGL不使用任何颜色数据进行渲染
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 4.渲染帧缓冲
    // 因为光是平行的定向光，因此光源的投影矩阵用正交投影，透视图将不会有变形
    float near_plane = 1.0f, far_plane = 7.5f;
    glm::vec3 lightPosition = glm::vec3(-2.0f, 3.0f, -1.0f); // 光照位置
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMat = lightProjection * lightView;
    glm::mat4 model(1.0f), view(1.0f);

    quadShader.use();
    quadShader.setInt("depthMap", 0);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    glEnable(GL_DEPTH_TEST);

    finalShadowShader.use();
    finalShadowShader.setInt("diffuseTexture", 0);
    finalShadowShader.setInt("shadowMap", 1);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // lightPosition = glm::vec3(lightPosition.x + glm::sin(glfwGetTime()) * 0.003, lightPosition.y, lightPosition.z);

        // 渲染深度贴图
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMat = lightProjection * lightView;
        shadowShader.use();
        shadowShader.setMat4("lightSpaceMat", lightSpaceMat);
        model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        shadowShader.setMat4("model", model);
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        drawMesh(floor);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0));
        shadowShader.setMat4("model", model);
        drawMesh(brick);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        finalShadowShader.use();
        finalShadowShader.setMat4("view", view);
        finalShadowShader.setMat4("projection", projection);
        finalShadowShader.setVec3("viewPos", camera.Position);
        finalShadowShader.setMat4("lightSpaceMat", lightSpaceMat);
        finalShadowShader.setVec3("lightPos", lightPosition);
        finalShadowShader.setFloat("uvScale", 4.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        finalShadowShader.setMat4("model", model);
        drawMesh(floor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickMap);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0));
        finalShadowShader.setMat4("model", model);
        finalShadowShader.setFloat("uvScale", 1.0);
        drawMesh(brick);


        // 显示深度贴图
        // *************************************************
        // quadShader.use();
        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, depthMap);
        // quadShader.setMat4("view", view);

        // // model = glm::mat4(1.0f);
        // model = glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0));
        // quadShader.setFloat("near_plane", near_plane);
        // quadShader.setFloat("far_plane", far_plane);
        // quadShader.setMat4("model", model);
        // quadShader.setMat4("projection", projection);
        // drawMesh(quadGeometry);
        // *************************************************

        drawLightObject(lightObjectShader, pointLightGeometry, lightPosition);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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

void drawMesh(BufferGeometry &geometry)
{
    glBindVertexArray(geometry.vao);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// 绘制灯光物体
void drawLightObject(Shader shader, BufferGeometry geometry, glm::vec3 position)
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);
    projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);

    // // 绘制灯光物体
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    model = glm::mat4(1.0f);
    model = glm::translate(model, position);

    shader.setMat4("model", model);
    shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    drawMesh(geometry);
}