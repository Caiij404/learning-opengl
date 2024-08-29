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
#include <tool/glm_io.hpp>
#include <tool/mySpace.h>
#include <tool/myImGui.hpp>
using namespace std;
using namespace mySpace;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void drawMesh(BufferGeometry &geometry);

std::string Shader::dirName;

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(glm::vec3(0.0, 1.0, 6.0));
float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;
bool flag = false;
int renderType = 0;
int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    GLFWwindow *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL");

    myImGui gui(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader sceneShader("./shader/sceneVert.glsl", "./shader/sceneFrag.glsl");
    Shader lightShader("./shader/lightVert.glsl", "./shader/lightFrag.glsl");
    Shader frameShader("./shader/frame_buffer_quad_vert.glsl", "./shader/frame_buffer_quad_frag.glsl");

    PlaneGeometry planeGeometry(10.0f, 10.0f, 10.0f, 10.0f);
    BoxGeometry boxGeometry(1.0f, 1.0f, 1.0f);
    SphereGeometry sphereGeometry(0.1f, 10.0f, 10.0f);
    PlaneGeometry grassGeometry;

    GLuint brick = loadTexture("./static/texture/brick_diffuse.jpg");
    GLuint board = loadTexture("./static/texture/wall.jpg");
    GLuint grass = loadTexture("./static/texture/grass.png");
    GLuint wpng = loadTexture("./static/texture/blending_transparent_window.png");

    glm::vec3 lightPosition = glm::vec3(1.0, 2.5, 2.0); // 光照位置

    // float quadVertices[] = {
    //     // positions   // texCoords
    //     -1.0f, 1.0f, 0.0f, 1.0f,
    //     -1.0f, -1.0f, 0.0f, 0.0f,
    //     1.0f, -1.0f, 1.0f, 0.0f,

    //     -1.0f, 1.0f, 0.0f, 1.0f,
    //     1.0f, -1.0f, 1.0f, 0.0f,
    //     1.0f, 1.0f, 1.0f, 1.0f};

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, 0.25f, 0.0f, 0.0f,
        0.0f, 0.25f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.25f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 1.0f};
 
    // screen quad vao
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    // 点光源的位置
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f, 1.0f, 1.5f),
        glm::vec3(2.3f, 3.0f, -4.0f),
        glm::vec3(-4.0f, 2.0f, 1.0f),
        glm::vec3(1.4f, 2.0f, 0.3f)};

    // 点光源颜色
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)};

    glm::vec3 windowPosition[] = {
        glm::vec3(3, 1, 1),
        glm::vec3(-3, 1, -1),
        glm::vec3(3, 1, 2),
        glm::vec3(-3, 1, -2),
    };

    glm::vec3 grassPosition[] = {
        glm::vec3(0, 0.5, 0),
        glm::vec3(2, 0.5, 0),
        glm::vec3(4, 0.5, 0),
    };

    PlaneGeometry mirrorGeometry;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 帧缓冲
    // ***********************************************************************
    // 创建帧缓冲
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 创建纹理缓冲 和 纹理附件
    unsigned int texBuffer;
    glGenTextures(1, &texBuffer);
    glBindTexture(GL_TEXTURE_2D, texBuffer);
    // 这里和生成纹理的区别有几点：
    // 1.将维度设置为屏幕大小 2.最后一个纹理data参数传NULL。
    // 这个操作，仅仅给纹理分配了内存，而没有填充它。
    // 填充纹理将会在渲染到帧缓冲之后进行
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    // 将纹理附加到当前绑定的帧缓冲
    // 1.帧缓冲的目标（绘制、读取或二者都有） 2.想要附加的附件类型 3.附加的纹理类型 4.纹理 5.多级渐远纹理级别
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texBuffer, 0);

    // 希望能够进行深度测试和模板测试，创建一个深度附件到帧缓冲中。
    // ----------------------- 而只希望采样颜色缓冲，非其他缓冲，那创建一个渲染缓冲对象即可 -----------------------
    // 感觉LearnOpenGL翻译有问题，前面说不采样，用渲染缓冲对象即可，而这里又明显矛盾，不过渲染缓冲对象是深度和模板缓冲，确实没有进行采样。
    // 渲染缓冲对象
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    // 绑定渲染缓冲对象，之后所有渲染缓冲操作将影响当前的rbo
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    // 创建一个深度和模板渲染缓冲对象
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
    // 给渲染缓冲对象分配完内存后，就可以解绑它了。
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // 附加渲染缓冲对象到帧缓冲的深度和模板附件上
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    // 检查帧缓冲是否完整
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "ERROR::Framebuffer is not complete!" << std::endl;
    }
    // 记得要解绑，避免不小心渲染到错误的帧缓冲上
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 现在这个帧缓冲是完整的，我们只需要绑定这个帧缓冲对象，让渲染到帧缓冲的缓冲中而不是默认的帧缓冲中。
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;
        if (flag)
        {
            // 线框模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            // 普通模式
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        processInput(window);

        gui.newFrame();
        gui.createFrameInfo();
        gui.createButtonSwitchBool(flag);

        // phase 1
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        glm::mat4 model(1.0f);
        glm::vec3 tmpV(1.0f, 0.0f, 0.0f);
        float radius = 5.0f;
        float camX = sin(currentFrame * 0.5) * radius;
        float camZ = cos(currentFrame * 0.5) * radius;
        pointLightPositions[0].x = camX;
        pointLightPositions[0].z = camZ;

        glm::mat4 view = glm::lookAt(camera.Position, camera.Position - camera.Front, camera.Up);
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

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
        model = glm::rotate(model, glm::radians(-90.0f), tmpV);

        sceneShader.setFloat("uvScale", 4.0);
        sceneShader.setMat4("model", model);

        // 绘制地板
        glBindTexture(GL_TEXTURE_2D, board);
        drawMesh(planeGeometry);

        // 绘制砖块
        glBindTexture(GL_TEXTURE_2D, brick);
        // 启用面剔除
        glEnable(GL_CULL_FACE);
        // 剔除背向面
        glCullFace(GL_BACK);

        model = glm::mat4(1.0f);
        tmpV = glm::vec3(2.0f);
        model = glm::scale(model, tmpV);
        tmpV = glm::vec3(1.0, 0.5, -1.0);
        model = glm::translate(model, tmpV);
        sceneShader.setMat4("model", model);
        sceneShader.setFloat("uvScale", 1.0f);
        drawMesh(boxGeometry);

        model = glm::mat4(1.0f);
        tmpV = glm::vec3(-1.0f, 0.5f, 2.0f);
        model = glm::translate(model, tmpV);
        sceneShader.setMat4("model", model);
        drawMesh(boxGeometry);

        glDisable(GL_CULL_FACE);

        // 绘制灯光物体
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glm::vec3 lightPos = glm::vec3(lightPosition.x * glm::sin(currentFrame) * 2.0, lightPosition.y, lightPosition.z * glm::cos(currentFrame) * 2.0f);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        lightShader.setMat4("model", model);
        tmpV = glm::vec3(1.0f, 1.0f, 1.0f);
        lightShader.setVec3("lightColor", tmpV);

        drawMesh(sphereGeometry);

        for (int i = 0; i < 4; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", pointLightColors[i]);
            drawMesh(sphereGeometry);
        }

        // 绘制草地
        sceneShader.use();
        sceneShader.setBool("isRGBA", true);
        glBindTexture(GL_TEXTURE_2D, grass);

        model = glm::mat4(1.0f);
        model = glm::translate(model, grassPosition[0]);
        sceneShader.setMat4("model", model);
        drawMesh(grassGeometry);

        model = glm::mat4(1.0f);
        model = glm::translate(model, grassPosition[1]);
        sceneShader.setMat4("model", model);
        drawMesh(grassGeometry);

        model = glm::mat4(1.0f);
        model = glm::translate(model, grassPosition[2]);
        sceneShader.setMat4("model", model);
        drawMesh(grassGeometry);

        // 绘制透明窗户
        glBindTexture(GL_TEXTURE_2D, wpng);
        map<float, glm::vec3> sorted;
        for (int i = 0; i < 4; ++i)
        {
            float dist = glm::length(windowPosition[i] - camera.Position);
            sorted[dist] = windowPosition[i];
        }

        // // 从远处开始渲染
        for (auto i = sorted.rbegin(); i != sorted.rend(); ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, i->second);
            model = glm::scale(model, glm::vec3(2.0f));
            sceneShader.setMat4("model", model);
            drawMesh(grassGeometry);
        }
        sceneShader.setBool("isRGBA", false);

        // -----------------------------------------------------------------------------------------------
        // -----------------------------------------------------------------------------------------------

        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.1, 0.1, 0.1, 0.1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        view = camera.GetViewMatrix();
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

        model = glm::mat4(1.0f);
        tmpV = glm::vec3(1.0f, 0.0f, 0.0f);
        model = glm::rotate(model, glm::radians(-90.0f), tmpV);

        sceneShader.setFloat("uvScale", 4.0);
        sceneShader.setMat4("model", model);

        // 绘制地板
        glBindTexture(GL_TEXTURE_2D, board);
        drawMesh(planeGeometry);

        // 绘制砖块
        glBindTexture(GL_TEXTURE_2D, brick);
        // 启用面剔除
        glEnable(GL_CULL_FACE);
        // 剔除背向面
        glCullFace(GL_BACK);

        model = glm::mat4(1.0f);
        tmpV = glm::vec3(2.0f);
        model = glm::scale(model, tmpV);
        tmpV = glm::vec3(1.0, 0.5, -1.0);
        model = glm::translate(model, tmpV);
        sceneShader.setMat4("model", model);
        sceneShader.setFloat("uvScale", 1.0f);
        drawMesh(boxGeometry);

        model = glm::mat4(1.0f);
        tmpV = glm::vec3(-1.0f, 0.5f, 2.0f);
        model = glm::translate(model, tmpV);
        sceneShader.setMat4("model", model);
        drawMesh(boxGeometry);

        glDisable(GL_CULL_FACE);

        // 绘制灯光物体
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        lightShader.setMat4("model", model);
        tmpV = glm::vec3(1.0f, 1.0f, 1.0f);
        lightShader.setVec3("lightColor", tmpV);

        drawMesh(sphereGeometry);

        for (int i = 0; i < 4; ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", pointLightColors[i]);
            drawMesh(sphereGeometry);
        }

        // 绘制草地
        sceneShader.use();
        sceneShader.setBool("isRGBA", true);
        glBindTexture(GL_TEXTURE_2D, grass);

        model = glm::mat4(1.0f);
        model = glm::translate(model, grassPosition[0]);
        sceneShader.setMat4("model", model);
        drawMesh(grassGeometry);

        model = glm::mat4(1.0f);
        model = glm::translate(model, grassPosition[1]);
        sceneShader.setMat4("model", model);
        drawMesh(grassGeometry);

        model = glm::mat4(1.0f);
        model = glm::translate(model, grassPosition[2]);
        sceneShader.setMat4("model", model);
        drawMesh(grassGeometry);

        // 从远处开始渲染
        glBindTexture(GL_TEXTURE_2D, wpng);
        for (auto i = sorted.rbegin(); i != sorted.rend(); ++i)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, i->second);
            model = glm::scale(model, glm::vec3(2.0f));
            sceneShader.setMat4("model", model);
            drawMesh(grassGeometry);
        }
        sceneShader.setBool("isRGBA", false);

        // -----------------------------------------------------------------------------------------------
        // -----------------------------------------------------------------------------------------------

        glDisable(GL_DEPTH_TEST);
        frameShader.use();
        frameShader.setInt("renderType", 0);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        // drawMesh(mirrorGeometry);
        // glBindVertexArray(mirrorGeometry.vao);
        // glDrawElements(GL_TRIANGLES, mirrorGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 渲染 gui
        gui.render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteFramebuffers(1, &fbo);
    planeGeometry.dispose();
    boxGeometry.dispose();
    sphereGeometry.dispose();
    grassGeometry.dispose();
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    Camera_KeyBoardAction action;
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        renderType = 1;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        renderType = 2;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        renderType = 3;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
    {
        renderType = 4;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
    {
        renderType = 5;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
    {
        renderType = 6;
    }

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
    // if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    //     action = ROTATEL;
    // if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    //     action = ROTATER;
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
        // camera.ProcessMouseMovement(xoffset, yoffset, TRANSLATION);
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

void drawMesh(BufferGeometry &geometry)
{
    glBindVertexArray(geometry.vao);
    glDrawElements(GL_TRIANGLES, geometry.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}