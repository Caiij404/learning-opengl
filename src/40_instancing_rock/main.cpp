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

std::string Shader::dirName;

// delta time
float deltaTime = 0.0f;
float lastTime = 0.0f;

Camera camera(glm::vec3(0.0, 1.0, 30.0));
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

    Shader sceneShader("./shader/scene_vert.glsl", "./shader/frag.glsl");
    Shader instanceShader("./shader/vert.glsl", "./shader/frag.glsl");

    Model rock("./static/model/rock/rock.obj");
    Model planet("./static/model/planet/planet.obj");

    int amount = 100000;
    glm::mat4 *modelMatrices = new glm::mat4[amount];
    // 初始化随机种子
    srand(glfwGetTime());
    float radius = 20.0f;
    float offset = 1.5f;
    for (int i = 0; i < amount; ++i)
    {
        glm::mat4 model(1.0f);
        // 1.位移：分布在半径为radius的圆形上，偏移范围是[-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4;
        displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;

        // model = glm::translate(model, glm::vec3(x,y,z));
        // float scale = (rand() % 20) / 1000.0f + 0.001;
        // model = glm::scale(model, glm::vec3(scale));
        // float rotAngle = (rand() % 360);
        // model = glm::rotate(model, rotAngle, glm::vec3(0.4,0.6,0.8));
        // modelMatrices[i] = model;

        glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
        glm::mat4 r = glm::rotate(glm::mat4(1.0f), (float)(rand() % 360), glm::vec3(0.4, 0.6, 0.8));
        glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3((rand() % 20) / 1000.0f + 0.01));
        // 为啥顺序是这个？
        modelMatrices[i] = t * s * r;
    }

    // 设置实例化数组
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    cout<<rock.meshes.size() << "--------------------------"<<endl;
    for (unsigned int i = 0; i < rock.meshes.size(); ++i)
    {
        unsigned int vao = rock.meshes[i].vao;
        glBindVertexArray(vao);
        // 顶点属性
        GLsizei vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        glBindVertexArray(0);
    }

    glm::mat4 projection = glm::perspective(camera.Zoom, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

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

        // drawSkyBox(skyBoxShader, skyBox, skyBoxTextrue);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model(1.0f);
        sceneShader.use();
        sceneShader.setMat4("projection", projection);
        sceneShader.setMat4("view", view);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 4.0f));
        sceneShader.setMat4("model", model);

        planet.Draw(sceneShader);

        instanceShader.use();
        instanceShader.setMat4("projection", projection);
        instanceShader.setMat4("view", view);
        instanceShader.setInt("diffuseTexture", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
        for (int i = 0; i < rock.meshes.size(); ++i)
        {
            glBindVertexArray(rock.meshes[i].vao);
            glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
        }

        gui.createFrameInfo();
        gui.render();
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