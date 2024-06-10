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

std::ostream &operator<<(std::ostream &os, const glm::vec4 &v4)
{
    os << "glm::vec4 ---- " << "X: " << v4.x << " Y: " << v4.y << " Z: " << v4.z << " W: " << v4.w << std::endl;
}



#endif