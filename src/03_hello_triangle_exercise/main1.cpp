// 1.绘制两个彼此相连的三角形 
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
//                                  "layout (location = 0) in vec3 aPos;\n"
//                                  "void main()\n"
//                                  "{\n"
//                                  "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
//                                  "}\0";
// const char *fragmentShaderSource03 = "#version 330 core\n"
//                                    "out vec4 FragColor;\n"
//                                    "void main()\n"
//                                    "{\n"
//                                    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
//                                    "}\n\0";

// int main()
// {
//     // glfw: initialize and configure
//     // ------------------------------
//     glfwInit();
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

//     float vertices[] = {
//         // first triangle
//         -0.9f, -0.5f, 0.0f,  // left 
//         -0.0f, -0.5f, 0.0f,  // right
//         -0.45f, 0.5f, 0.0f,  // top 
//         // second triangle
//          0.0f, -0.5f, 0.0f,  // left
//          0.9f, -0.5f, 0.0f,  // right
//          0.45f, 0.5f, 0.0f   // top 
//     };

//     GLuint vbo, vao;
//     glGenVertexArrays(1, &vao);
//     glGenBuffers(1, &vbo);

//     // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes
//     glBindVertexArray(vao);

//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
//     glEnableVertexAttribArray(0);

//     // note that this is allowed, the call to glVertexAttribPointer registered vbo as ther vertex attribute's bound vertex buffer object so afterward we can safely unbind
//     glBindBuffer(GL_ARRAY_BUFFER,0);

//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

//     while(!glfwWindowShouldClose(window))
//     {
//         processInput(window);

//         glClearColor(0.2, 0.3, 0.3, 1.0);
//         glClear(GL_COLOR_BUFFER_BIT);

//         glUseProgram(shaderProgram);
//         glDrawArrays(GL_TRIANGLES, 0, 6);


//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glDeleteVertexArrays(1, &vao);
//     glDeleteBuffers(1, &vbo);
//     glDeleteProgram(shaderProgram);
//     glfwTerminate();
//     return 0;
// }


// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// // glfw: whenever the window size changed (by OS or user resize) this callback function executes
// // ---------------------------------------------------------------------------------------------
// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and 
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }