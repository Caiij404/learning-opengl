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

    Shader depthMapShader("./shader/shadow_map_vert.glsl", "./shader/shadow_map_frag.glsl", "./shader/shadow_map_geom.glsl");
    Shader sceneShader("./shader/shadow_final_vert.glsl", "./shader/shadow_final_frag.glsl");
    Shader lightObjectShader("./shader/light_object_vert.glsl", "./shader/light_object_frag.glsl");

    GLuint floorMap = loadTexture("./static/texture/wood.png");
    GLuint brickMap = loadTexture("./static/texture/brick_diffuse.jpg"); // 砖墙

    // PlaneGeometry floor(15.0f, 15.0f);
    // BoxGeometry brick(1.0f, 1.0f, 1.0f);
    BoxGeometry boxGeometry(1.0, 1.0, 1.0);
    // BoxGeometry floorGeometry(10.0, 0.0001, 10.0);
    SphereGeometry pointLightGeometry(0.06, 10.0, 10.0); // 点光源位置显示
    // PlaneGeometry quadGeometry(6.0, 6.0);                // 测试面板

    // 定义是个不同的箱子位置
    vector<glm::vec3> cubePositions{
        glm::vec3(2.3f, -2.0f, -1.0),
        glm::vec3(2.0f, 2.3f, 1.0),
        glm::vec3(-2.5f, -1.0f, 0.0),
        glm::vec3(-1.5f, 1.0f, 1.5),
        glm::vec3(-1.5f, 2.0f, -2.5)};

    // 阴影贴图
    // 1.创建帧缓冲
    GLuint depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);

    // 2.创建cube纹理
    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthCubeMap;
    glGenTextures(1, &depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    // 这个参数写成2D的，一直出不来阴影，找了半天，倒是找出了好几处其他错误.....
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // 3.把生成的纹理作为帧缓冲的深度缓冲
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    // 因为只关心深度值而不关系颜色，但不包含颜色缓冲的帧缓冲是不完整的，因此需要显示explicitly告诉OpenGL不使用任何颜色数据进行渲染
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 4.渲染帧缓冲
    // 这次使用的是点光源，空间中一个点，所以用透视投影矩阵会更有意义。
    // 投影矩阵在每个方向上都是不变的，6个面可以使用同一个
    float aspect = (float)SHADOW_WIDTH / SHADOW_HEIGHT;
    float near = 1.0f, far = 25.0f, fov = glm::radians(90.0f);
    glm::vec3 lightPosition = glm::vec3(-2.0f, 0.0f, 0.0f); // 光照位置
    // glm::mat4 lightProjection = glm::perspective(fov, aspect, near, far);
    // // 6个方向上的lookAt，按右 左 上 下 近 远顺序生成
    // std::vector<glm::mat4> lightViews;
    // lightViews.push_back(glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    // lightViews.push_back(glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    // lightViews.push_back(glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    // lightViews.push_back(glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    // lightViews.push_back(glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    // lightViews.push_back(glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

    // std::vector<glm::mat4> shadowProj;
    // for (int i = 0; i < lightViews.size(); ++i)
    // {
    //     shadowProj.push_back(lightProjection * lightViews[i]);
    // }

    glm::mat4 model(1.0f), view(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    glEnable(GL_DEPTH_TEST);

    sceneShader.use();
    sceneShader.setInt("diffuseTexture", 0);
    sceneShader.setInt("depthMap", 1);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        float timeValue = glfwGetTime();
        gui.newFrame();
        gui.createFrameInfo();

        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);

        // 渲染深度贴图
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        glm::mat4 shadowProj = glm::perspective(fov, aspect, near, far);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosition, lightPosition + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        depthMapShader.use();
        for (int i = 0; i < shadowTransforms.size(); ++i)
        {
            depthMapShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        }
        depthMapShader.setFloat("far_plane", far);
        depthMapShader.setVec3("lightPos", lightPosition);

        model = glm::scale(glm::mat4(1.0f), glm::vec3(7, 7, 7));
        depthMapShader.setMat4("model", model);
        // 绘制大箱子
        drawMesh(boxGeometry);

        // 绘制多个箱子
        for (int i = 0; i < cubePositions.size(); ++i)
        {
            // model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            // float angle = 10.0f * i;
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 2.0f, 3.0f));
            // depthMapShader.setMat4("model", model);
            // drawMesh(boxGeometry);

            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 10.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            depthMapShader.setMat4("model", model);
            drawMesh(boxGeometry);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();

        sceneShader.use();
        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);
        sceneShader.setVec3("viewPos", camera.Position);
        sceneShader.setFloat("far_plane", far);
        sceneShader.setVec3("lightPos", lightPosition);
        // sceneShader.setFloat("uvScale", 4.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

        model = glm::scale(glm::mat4(1.0f), glm::vec3(7, 7, 7));
        sceneShader.setMat4("model", model);
        sceneShader.setFloat("uvScale", 1.0f);
        sceneShader.setInt("reverse_normal", -1);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        drawMesh(boxGeometry);
        glDisable(GL_CULL_FACE);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brickMap);

        for (int i = 0; i < cubePositions.size(); ++i)
        {
            // float angle = 10.0f * i;
            // model = glm::translate(glm::mat4(1.0f), cubePositions[i]);
            // model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 2.0f, 3.0f));
            // sceneShader.setMat4("model", model);
            // sceneShader.setInt("reverse_normal", 1);
            // drawMesh(boxGeometry);

            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 10.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));

            sceneShader.setMat4("model", model);
            sceneShader.setInt("reverse_normal", 1);
            drawMesh(boxGeometry);
        }

        drawLightObject(lightObjectShader, pointLightGeometry, lightPosition);

        gui.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    boxGeometry.dispose();
    pointLightGeometry.dispose();
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