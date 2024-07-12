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
bool stopPainting = false;
bool flag = false;
int renderType = 0;
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

    float quadVertices[] = {
        // positions   // texCoords
        -1.0f, 1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f};

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

    float myNear = 0.1;
    float x = 0.0f, y = 0.0f, z = 0.0f;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 帧缓冲
    // ***********************************************************************
    // 创建帧缓冲
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 创建纹理附件
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
    // 而只希望采样颜色缓冲，非其他缓冲，那创建一个渲染缓冲对象即可
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
        // 帧缓冲章节，《后期处理》前的内容，可通过鼠标左右键按住并滑动一段距离的方式来展示。
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

        // 显示帧率
        // *************************************************************************
        int fps_value = (int)round(ImGui::GetIO().Framerate);
        int ms_value = (int)round(1000.0f / ImGui::GetIO().Framerate);

        std::string FPS = std::to_string(fps_value);
        std::string ms = std::to_string(ms_value);
        std::string newTitle = "LearnOpenGL - " + ms + " ms/frame" + FPS;
        glfwSetWindowTitle(window, newTitle.c_str());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // *************************************************************************

        // *********************************像素风尝试*********************************
        // 缩小视口和颜色纹理->在缩小后的纹理上绘制场景->还原视口->绘制帧缓冲
        // 缩放
        const float buf_scale = 5.0f;
        const int buf_width = SCREEN_WIDTH / buf_scale;
        const int buf_height = SCREEN_HEIGHT / buf_scale;

        // 重新设置颜色缓冲
        glBindTexture(GL_TEXTURE_2D, texBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, buf_width, buf_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, buf_width, buf_height);
        glViewport(0, 0, buf_width, buf_height);
        // *********************************像素风尝试*********************************

        // part 1
        // bind to framebuffer and draw scene as we normally would to color texture
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        // 将场景绘制到帧缓冲中
        {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

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
        // 启用面剔除
        glEnable(GL_CULL_FACE);
        // 剔除背向面
        glCullFace(GL_BACK);

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

        glDisable(GL_CULL_FACE);

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

        // 绘制草地
        sceneShader.use();
        sceneShader.setBool("isRGBA", true);
        glBindTexture(GL_TEXTURE_2D, grass);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0.5, 0));
        sceneShader.setMat4("model", model);
        glBindVertexArray(grassGeometry.vao);
        glDrawElements(GL_TRIANGLES, grassGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(2, 0.5, 0));
        sceneShader.setMat4("model", model);
        glBindVertexArray(grassGeometry.vao);
        glDrawElements(GL_TRIANGLES, grassGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(4, 0.5, 0));
        sceneShader.setMat4("model", model);
        glBindVertexArray(grassGeometry.vao);
        glDrawElements(GL_TRIANGLES, grassGeometry.indices.size(), GL_UNSIGNED_INT, 0);

        // 绘制透明窗户
        glBindTexture(GL_TEXTURE_2D, wpng);
        map<float, glm::vec3> sorted;
        for (int i = 0; i < 4; ++i)
        {
            float distance = glm::length(windowPosition[i] - camera.Position);
            sorted[distance] = windowPosition[i];
        }

        // 从远处的透明窗户开始渲染
        for (auto i = sorted.rbegin(); i != sorted.rend(); ++i)
        {
            model = glm::translate(glm::mat4(1.0f), i->second);
            model = glm::scale(model, glm::vec3(2.0f));
            sceneShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, grassGeometry.indices.size(), GL_UNSIGNED_INT, 0);
        }
        sceneShader.setBool("isRGBA", false);

        }

        // *********************************像素风尝试*********************************
        glViewport(0,0,SCREEN_WIDTH, SCREEN_HEIGHT);
        // *********************************像素风尝试*********************************


        // now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        frameShader.use();
        frameShader.setInt("renderType", renderType);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // 渲染 gui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        stopPainting = !stopPainting;
    }

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
        camera.ProcessMouseMovement(xoffset, yoffset, TRANSLATION);
        flag = true;
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        camera.ProcessMouseMovement(xoffset, yoffset, ROTATION);
        flag = false;
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