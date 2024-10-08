#ifndef GLM_IO_H
#define GLM_IO_H
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
std::ostream &operator<<(std::ostream &os, const glm::mat4 &matrix)
{
    // 将矩阵转换为指向float的指针
    const float *pMatrix = (const float *)glm::value_ptr(matrix);

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            float t = abs(pMatrix[j * 4 + i]) < 1e-7 ? 0 : pMatrix[j * 4 + i];
            os << t << " ";
        }
        os << std::endl;
    }
    os << "----------------------------------" << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::mat3 &matrix)
{
    // 将矩阵转换为指向float的指针
    const float *pMatrix = (const float *)glm::value_ptr(matrix);

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            float t = abs(pMatrix[j * 3 + i]) < 1e-7 ? 0 : pMatrix[j * 3 + i];
            os << t << " ";
        }
        os << std::endl;
    }
    os << "----------------------------------" << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::vec4 &v4)
{
    os << "glm::vec4 ---- " << "X: " << v4.x << " Y: " << v4.y << " Z: " << v4.z << " W: " << v4.w << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, const glm::vec3 &v3)
{
    os << "glm::vec3 ---- " << "X: " << v3.x << " Y: " << v3.y << " Z: " << v3.z << std::endl;
    return os;
}

#endif