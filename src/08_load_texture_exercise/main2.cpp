// // 练习2
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <iostream>
// #include <tool/shader.h>

// #define STB_IMAGE_IMPLEMENTATION
// #include <tool/stb_image.h>

// using namespace std;

// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
// void processInput(GLFWwindow *window);
// std::string Shader::dirName;
// const unsigned int SCR_WIDTH = 1600;
// const unsigned int SCR_HEIGHT = 1200;

// int main(int argc, char *argv[])
// {
//     Shader::dirName = argv[1];
//     glfwInit();
//     // 设置主要和次要版本
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

//     // 创建窗口对象
//     GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
//     if (window == NULL)
//     {
//         std::cout << "Failed to create GLFW window" << std::endl;
//         glfwTerminate();
//         return -1;
//     }
//     glfwMakeContextCurrent(window);

//     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//     {
//         std::cout << "Failed to initialize GLAD" << std::endl;
//         return -1;
//     }
//     // 设置视口
//     glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
//     glEnable(GL_PROGRAM_POINT_SIZE);

//     // 注册窗口变化监听
//     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

//     Shader ourShader("./shader/vertex.glsl", "./shader/fragment.glsl");

//     // 练习2：设定纹理坐标范围在0-2之间，实现箱子角落放置4个笑脸
//     // 顶点数据
//     float vertices[] = {
//         // ---位置---       --- 颜色 ---        --纹理坐标--
//         0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,   // 右上
//         0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f,  // 右下
//         -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // 左下
//         -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f,  // 左上
//     };

//     // 索引数据
//     unsigned int indices[] = {
//         0, 1, 3,
//         1, 2, 3};

//     // 创建缓冲对象
//     unsigned int vbo, ebo, vao;
//     glGenVertexArrays(1, &vao);
//     glGenBuffers(1, &vbo);
//     glGenBuffers(1, &ebo);

//     // 绑定vao
//     glBindVertexArray(vao);

//     // 绑定vbo
//     glBindBuffer(GL_ARRAY_BUFFER, vbo);
//     // 填充vbo数据
//     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

//     // 绑定ebo 填充索引数据
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

//     // 设置顶点位置属性指针
//     // 1.location = 0；2.属性大小 vec；3.数据类型；4.是否标准化；5.步长；6.偏移
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);

//     // 设置顶点颜色属性指针
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);

//     // 设置顶点纹理坐标属性指针
//     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
//     glEnableVertexAttribArray(2);

//     // ______生成纹理______

//     // 纹理对象
//     // 参数：纹理个数、存放ID的数组
//     GLuint textures[2];
//     glGenTextures(2, textures);
//     // 绑定纹理前先激活纹理单元Texture Unit（纹理的位置值）。一个纹理的默认纹理单元是0，0号默认激活
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, textures[0]);

//     // 设置环绕和过滤方式
//     // 练习2：试试其他纹理环绕方式
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     // flip the image vertically, so the first pixel in the output array is the bottom left
//     // 图像y轴翻转，使得输出数组的第一个像素位于左下角
//     stbi_set_flip_vertically_on_load(true);

//     // 加载图像
//     int width, height, nrChannels;
//     // stbi_load会将图像的宽度、高度和颜色通道个数填充到这三个变量中，后续生成纹理时会用到
//     unsigned char *data = stbi_load("./static/texture/container.jpg", &width, &height, &nrChannels, 0);

//     if (data)
//     {
//         // 通过glTexImage2D生成纹理，将纹理图像附加在纹理对象texture上
//         // 纹理目标(Target) 多级渐远纹理级别 纹理存储形式(我们图像只有rgb信息) 图像宽 图像高 总是0 源图的格式 源图数据类型 图像数据
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

//         // 设置多级渐远纹理
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     else
//     {
//         std::cout << "Failed to load texture" << std::endl;
//     }
//     stbi_image_free(data);

//     glActiveTexture(GL_TEXTURE1);
//     glBindTexture(GL_TEXTURE_2D, textures[1]);
//     // 设置 环绕 和 过滤 方式
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     // 第二张图
//     data = stbi_load("./static/texture/awesomeface.png", &width, &height, &nrChannels, 0);
//     if (data)
//     {
//         // !!!!!! png是四通道图，GL_RGBA
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//         glGenerateMipmap(GL_TEXTURE_2D);
//     }
//     stbi_image_free(data);

//     // glUniform1i 是数字1，不是字母l
//     // 纹理单元数量超过1个，需要glUniform1i来设置每个采样器的方式告诉OpenGL每个着色器属于哪个纹理单元
//     ourShader.use(); // 不要忘记在设置uniform变量之前激活着色器程序！
//     ourShader.setInt("texture1", 0);
//     ourShader.setInt("texture2", 1);

//     while (!glfwWindowShouldClose(window))
//     {
//         processInput(window);

//         // 渲染指令
//         glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
//         glClear(GL_COLOR_BUFFER_BIT);

//         ourShader.use();

//         // glActiveTexture(GL_TEXTURE0);
//         // glBindTexture(GL_TEXTURE_2D, textures[0]);

//         // glActiveTexture(GL_TEXTURE1);
//         // glBindTexture(GL_TEXTURE_2D, textures[1]);

//         glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//         glfwSwapBuffers(window);
//         glfwPollEvents();
//     }

//     glDeleteVertexArrays(1, &vao);
//     glDeleteBuffers(1, &vbo);
//     glDeleteBuffers(1, &ebo);
//     glfwTerminate();
//     return 0;
// }

// void processInput(GLFWwindow *window)
// {
//     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }

// void framebuffer_size_callback(GLFWwindow *window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }