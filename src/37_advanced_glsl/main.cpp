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

#include <tool/mySpace.h>
using namespace std;
using namespace mySpace;
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void processInput(GLFWwindow *window);
void drawSkyBox(Shader shader, BoxGeometry geometry, unsigned int cubeMap);

std::string Shader::dirName;

float deltaTime = 0.0f;
float lastTime = 0.0f;
float lastX = SCREEN_WIDTH / 2.0f; // 鼠标上一帧的位置
float lastY = SCREEN_HEIGHT / 2.0f;

Camera camera(glm::vec3(0.0, 1.0, 6.0));

int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    GLFWwindow *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL");

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader skyBoxShader("./shader/cube_map_vert.glsl", "./shader/cube_map_frag.glsl");
    Shader pointShader("./shader/point_vert.glsl", "./shader/point_frag.glsl");
    Shader sceneShader1("./shader/scene_vert.glsl", "./shader/scene1_frag.glsl");
    Shader sceneShader2("./shader/scene_vert.glsl", "./shader/scene2_frag.glsl");
    Shader sceneShader3("./shader/scene_vert.glsl", "./shader/scene3_frag.glsl");
    Shader sceneShader4("./shader/scene_vert.glsl", "./shader/scene4_frag.glsl");

    BoxGeometry skyBox(1.0, 1.0, 1.0);
    vector<const char *> texture_faces = {
        "./static/texture/skybox/right.jpg",
        "./static/texture/skybox/left.jpg",
        "./static/texture/skybox/top.jpg",
        "./static/texture/skybox/bottom.jpg",
        "./static/texture/skybox/back.jpg",
        "./static/texture/skybox/front.jpg"};
    GLuint skyBoxTexture = loadCubeTexture(texture_faces);

    SphereGeometry sphere(2, 20.0, 20.0);

    BoxGeometry box(1.0f, 1.0f, 1.0f);
    GLuint uvMap = loadTexture("./static/texture/uv_grid_directx.jpg");
    GLuint triMap = loadTexture("./static/texture/tri_pattern.jpg");

    // 创建uniform缓冲对象
    GLuint ubo;
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // 分配内存 data为NULL
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // 给ubo对象的绑定点0的内存，声明大小
    // 1.缓冲类型 2.绑定点 3.缓冲对象 4.偏移量 5.大小
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
    // 这接口也可以绑定
    // glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

    // 获取块索引
    GLuint uniformBlockIndex_1 = glGetUniformBlockIndex(sceneShader1.ID, "Matrices");
    GLuint uniformBlockIndex_2 = glGetUniformBlockIndex(sceneShader2.ID, "Matrices");
    GLuint uniformBlockIndex_3 = glGetUniformBlockIndex(sceneShader3.ID, "Matrices");
    GLuint uniformBlockIndex_4 = glGetUniformBlockIndex(sceneShader4.ID, "Matrices");

    // 将顶点着色器中uniform块设置绑定点为0
    glUniformBlockBinding(sceneShader1.ID, uniformBlockIndex_1, 0);
    glUniformBlockBinding(sceneShader2.ID, uniformBlockIndex_2, 0);
    glUniformBlockBinding(sceneShader3.ID, uniformBlockIndex_3, 0);
    glUniformBlockBinding(sceneShader4.ID, uniformBlockIndex_4, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    // 向ubo中填充投影矩阵
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2, 0.1, 0.4, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastTime;
        lastTime = currentFrame;

        drawSkyBox(skyBoxShader, skyBox, skyBoxTexture);

        glm::mat4 view = camera.GetViewMatrix();
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, uvMap);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, triMap);

        glBindVertexArray(box.vao);

        float rotate = glfwGetTime() * 0.2f;
        glm::qua<float> qu = glm::qua<float>(glm::vec3(rotate, rotate, rotate));

        glm::mat4 model(1.0f);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, 0.75, 0.0));
        model = model * glm::mat4_cast(qu);
        sceneShader1.use();
        sceneShader1.setMat4("model", model);
        sceneShader1.setInt("uvMap", 0);
        sceneShader1.setInt("triMap", 1);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.75, 0.75, 0.0));
        model = model * glm::mat4_cast(qu);
        sceneShader2.use();
        sceneShader2.setMat4("model", model);
        sceneShader2.setInt("uvMap", 0);
        sceneShader2.setInt("triMap", 1);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(0.75, -0.75, 0.0));
        model = model * glm::mat4_cast(qu);
        sceneShader3.use();
        sceneShader3.setMat4("model", model);
        sceneShader3.setInt("uvMap", 0);
        sceneShader3.setInt("triMap", 1);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.75, -0.75, 0.0));
        model = model * glm::mat4_cast(qu);
        sceneShader4.use();
        sceneShader4.setMat4("model", model);
        sceneShader4.setInt("uvMap", 0);
        sceneShader4.setInt("triMap", 1);
        glDrawElements(GL_TRIANGLES, box.indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    box.dispose();
    skyBox.dispose();
    sphere.dispose();
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