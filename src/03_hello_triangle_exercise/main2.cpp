// 通过不同的vao和vbo绘制两个三角形
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <iostream>

// #include "Utils/utils.h"

// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// void processInput(GLFWwindow *window);

// // settings
// const GLuint SCR_WIDTH = 800;
// const GLuint SCR_HEIGHT = 600;

// const char *vertexShaderSource03 = "#version 330 core\n"
//                                    "layout (location = 0) in vec3 aPos;\n"
//                                    "void main()\n"
//                                    "{\n"
//                                    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//                                    "}\0";
// const char *fragmentShaderSource03 = "#version 330 core\n"
//                                      "out vec4 FragColor;\n"
//                                      "void main()\n"
//                                      "{\n"
//                                      "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//                                      "}\n\0";

// int main()
// {
//     // glfw: initialize and configure
//     // ------------------------------
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     // 通过不同的vao和vbo绘制两个三角形
//     std::cout << "-------------------" << "Draw 2 triangles with different vao & vbo" << "-------------------" << std::endl;

// #ifdef __APPLE__
//     glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif

//     GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

//     // 通知GLFW将我们窗口的上下文设置为当前线程的主上下文
//     glfwMakeContextCurrent(window);
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     // glad: load all OpenGL function pointers
//     // 加载所有OpenGL函数指针
//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         glfwTerminate();
//         return -1;
//     }

//     GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertexShader, 1, &vertexShaderSource03, NULL);
//     glCompileShader(vertexShader);
//     // check for shader compile errors
//     compileInfoLog(vertexShader, SHADER);

//     GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragmentShader, 1, &fragmentShaderSource03, NULL);
//     glCompileShader(fragmentShader);
//     compileInfoLog(fragmentShader, SHADER);

//     // Link Shader
//     GLuint shaderProgram = glCreateProgram();
//     glAttachShader(shaderProgram, vertexShader);
//     glAttachShader(shaderProgram, fragmentShader);
//     glLinkProgram(shaderProgram);
//     // check errors
//     compileInfoLog(shaderProgram, SHADER_PROGRAM);

//     // 链接完就可以删除着色器
//     glDeleteShader(vertexShader);
//     glDeleteShader(fragmentShader);

//     float triangle1[] = {
//         // first triangle
//         -0.9f, -0.5f, 0.0f, // left
//         -0.0f, -0.5f, 0.0f, // right
//         -0.45f, 0.5f, 0.0f, // top

//     };

//     float triangle2[] = {
//         // second triangle
//         0.0f, -0.5f, 0.0f, // left
//         0.9f, -0.5f, 0.0f, // right
//         0.45f, 0.5f, 0.0f  // top
//     };

//     // ------------------------------------分割线------------------------------------------

//     std::cout << "authoritative code" << std::endl;
//     GLuint vao[2], vbo[2];
//     glGenVertexArrays(2, vao);
//     glGenBuffers(2, vbo);

//     glBindVertexArray(vao[0]);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
//     // glEnableVertexAttribArray，以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的。
//     glEnableVertexAttribArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);

//     glBindVertexArray(vao[1]);
//     glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, 0);

//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//     while (!glfwWindowShouldClose(window))
//     {
//         processInput(window);

//         glClearColor(0.2, 0.3, 0.3, 1.0);
//         glClear(GL_COLOR_BUFFER_BIT);

//         glUseProgram(shaderProgram);
//         glBindVertexArray(vao[0]);
//         // glUseProgram(shaderProgram);
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         glBindVertexArray(vao[1]);
//         glDrawArrays(GL_TRIANGLES, 0, 3);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glDeleteBuffers(2, vbo);
//     glDeleteVertexArrays(2, vao);
//     glDeleteProgram(shaderProgram);
//     glfwTerminate();
//     return 0;

//     // ------------------------------------分割线------------------------------------------

//     // 自己写的
//     // std::cout << "my code" << std::endl;
//     /*     GLuint vao1, vao2;
//         GLuint vbo1, vbo2;
//         glGenVertexArrays(1, &vao1);
//         glGenVertexArrays(1, &vao2);
//         glGenBuffers(1, &vbo1);
//         glGenBuffers(1, &vbo2);


//         glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


//         while(!glfwWindowShouldClose(window))
//         {
//             processInput(window);

//             glClearColor(0.2, 0.3, 0.3, 1.0);
//             glClear(GL_COLOR_BUFFER_BIT);

//             glUseProgram(shaderProgram);

//             glBindVertexArray(vao1);
//             glBindBuffer(GL_ARRAY_BUFFER, vbo1);
//             glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);

//             glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//             // glEnableVertexAttribArray，以顶点属性位置值作为参数，启用顶点属性；顶点属性默认是禁用的。
//             glEnableVertexAttribArray(0);

//             // glUseProgram(shaderProgram);
//             glDrawArrays(GL_TRIANGLES, 0, 3);

//             glBindVertexArray(vao2);
//             glBindBuffer(GL_ARRAY_BUFFER, vbo2);
//             glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);

//             glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//             glEnableVertexAttribArray(0);
//             glDrawArrays(GL_TRIANGLES, 0, 3);


//             glfwSwapBuffers(window);
//             glfwPollEvents();
//         }

//         glDeleteBuffers(1, &vbo1);
//         glDeleteBuffers(1, &vbo2);
//         glDeleteVertexArrays(1, &vao1);
//         glDeleteVertexArrays(1, &vao2);
//         glDeleteProgram(shaderProgram);
//         glfwTerminate();
//         return 0; */
// }

// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }