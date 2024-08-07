#ifndef MYSPACE_H
#define MYSPACE_H
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
using namespace std;
namespace mySpace
{

    int SCREEN_WIDTH = 800;
    int SCREEN_HEIGHT = 600;

    GLFWwindow *initWindow(int w, int h, const char *name)
    {
        glfwInit();
        // 设置主要和次要版本
        const char *glsl_version = "#version 330";

        // 片段着色器将作用域每一个采样点（采用4倍抗锯齿，则每个像素有4个片段（四个采样点））
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        // 窗口对象
        GLFWwindow *window = glfwCreateWindow(w, h, "LearnOpenGL", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return NULL;
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return NULL;
        }

        // 设置视口
        glViewport(0, 0, w, h);
        glEnable(GL_PROGRAM_POINT_SIZE);
        // glEnable(GL_BLEND);
        // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // glEnable(GL_DEPTH_TEST);

        // 多重采样抗锯齿
        glEnable(GL_MULTISAMPLE);

        return window;
    };

    unsigned int loadTexture(char const *path)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);

        // 图像y轴翻转
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            if (nrComponents == 4)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }
            else
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            }
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    };

    unsigned int loadCubeTexture(vector<const char *> faces)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        stbi_set_flip_vertically_on_load(false);

        int width, height, nrChannels;
        for (int i = 0; i < faces.size(); ++i)
        {
            unsigned char *data = stbi_load(faces[i], &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum format;
                if (nrChannels == 1)
                    format = GL_RED;
                else if (nrChannels == 3)
                    format = GL_RGB;
                else if (nrChannels == 4)
                    format = GL_RGBA;

                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            }
            else
            {
                std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            }
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        return textureID;
    }

    struct Material
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess = 64.0f;
    };

    struct AttenuationFactor
    {
        float constant = 1.0f;
        float linear = 0.09f;
        float quadratic = 0.032f;
    };

    struct DirLight
    {
        glm::vec3 direction;
        glm::vec3 ambient = glm::vec3(0.01f);
        glm::vec3 diffuse = glm::vec3(0.2f);
        glm::vec3 specular = glm::vec3(1.0f);
    };

    struct PointLight
    {
        glm::vec3 position;
        glm::vec3 ambient = glm::vec3(0.01f);
        glm::vec3 diffuse = glm::vec3(0.2f);
        glm::vec3 specular = glm::vec3(1.0f);
    };

    struct SpotLight
    {
        glm::vec3 position;
        glm::vec3 direction;

        glm::vec3 ambient = glm::vec3(0.0f);
        glm::vec3 diffuse = glm::vec3(1.0f);
        glm::vec3 specular = glm::vec3(1.0f);

        float cutoff;
        float outerCutoff;
    };
};

#endif