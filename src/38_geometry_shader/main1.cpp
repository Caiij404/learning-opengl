// 白顶房子

// #include <glad/glad.h>
// #include <GLFW/glfw3.h>

// #include <tool/shader.h>

// #include <iostream>
// using namespace std;

// void framebuffer_size_callback(GLFWwindow* window, int width, int height);


// #define STB_IMAGE_IMPLEMENTATION
// #include <tool/stb_image.h>
// #include <vector> 

// #include <tool/mySpace.h>
// using namespace mySpace;

// std::string Shader::dirName;

// int main(int argc, char *argv[])
// {
//     Shader::dirName = argv[1];
//     GLFWwindow *window = initWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Learn OpenGL");

//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     glEnable(GL_DEPTH_TEST);

//     Shader shader("./shader/vert.glsl", "./shader/frag.glsl", "./shader/geom.glsl");

//     float points[] = {
//         -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
//          0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
//          0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
//         -0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
//     };
//     unsigned int VBO, VAO;
//     glGenBuffers(1, &VBO);
//     glGenVertexArrays(1, &VAO);
//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
//     glBindVertexArray(0);

//     // render loop
//     // -----------
//     while (!glfwWindowShouldClose(window))
//     {
//         // render
//         // ------
//         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//         // draw points
//         shader.use();
//         glBindVertexArray(VAO);
//         glDrawArrays(GL_POINTS, 0, 4);

//         // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
//         // -------------------------------------------------------------------------------
//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     // optional: de-allocate all resources once they've outlived their purpose:
//     // ------------------------------------------------------------------------
//     glDeleteVertexArrays(1, &VAO);
//     glDeleteBuffers(1, &VBO);

//     glfwTerminate();
//     return 0;
    
//     return 0;
// }

// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and 
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }