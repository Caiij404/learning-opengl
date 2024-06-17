#ifndef BUFFER_GEOMETRY
#define BUFFER_GEOMETRY

#include <glm/glm.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>
#include <iostream>

using namespace std;

const float PI = glm::pi<float>();

// mesh.h中也有此定义
struct Vertex
{
    glm::vec3 Position;  // 顶点位置
    glm::vec3 Normal;    // 法线
    glm::vec2 TexCoords; // 纹理坐标

    glm::vec3 Tangent;   // 切线
    glm::vec3 Bitangent; // 副切线（双切线）
};

class BufferGeometry
{
public:
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int vao;

    void logParameters()
    {
        for (int i = 0; i < vertices.size(); ++i)
        {
            cout << "---------------------" << endl;
            cout << "vertex ->> x: " << vertices[i].Position.x << ", y: " << vertices[i].Position.y << ", z: " << vertices[i].Position.z << endl;
            cout << "normal ->> x: " << vertices[i].Normal.x << ", y: " << vertices[i].Normal.y << ", z: " << vertices[i].Normal.z << endl;
            cout << "texCoords ->> x: " << vertices[i].TexCoords.x << ", y: " << vertices[i].TexCoords.y << endl;
            cout << "tangent ->> x: " << vertices[i].Tangent.x << ", y: " << vertices[i].Tangent.y << ", z: " << vertices[i].Tangent.z << endl;
            cout << "bitangent ->> x: " << vertices[i].Bitangent.x << ", y: " << vertices[i].Bitangent.y << ", z: " << vertices[i].Bitangent.z << endl;
            cout << "---------------------" << endl;
        }
    }

    // 计算切线向量并添加到顶点属性中
    void computeTangents()
    {
    }

    void dispose()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
    }

private:
    glm::mat4 matrix = glm::mat4(1.0f);

protected:
    unsigned int vbo, ebo;

    void setupBuffers()
    {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);

        // vertex attribute
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_DYNAMIC_DRAW);

        // indices
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // 设置顶点属性指针
        // Position 
        // location = 0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(0));

        // Normal
        // location = 1
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

        // TexCoords
        // location = 2
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif