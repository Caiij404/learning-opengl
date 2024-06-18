#ifndef SPHERE_GEOMETRY
#define SPHERE_GEOMETRY

#include <geometry/BufferGeometry.h>
using namespace std;

class SphereGeometry : public BufferGeometry
{
public:
    // phiStart: specify horizontal start angle 指定水平起始角度
    // phiLength: specify horizontal sweep angle size, default is 2 * PI 指定水平扫掠角度大小，默认是2PI
    // phiStart：经度开始的弧度; phiLength：经度跨过的弧度; thetaStart：纬度开始的弧度; thetaLength：纬度跨过的弧度
    SphereGeometry(float radius = 1.0f, float widthSegments = 8.0f, float heightSegments = 6.0f, float phiStart = 0.0f, float phiLength = PI * 2.0f, float thetaStart = 0.0f, float thetaLength = PI)
    {
        const float thetaEnd = glm::min(thetaStart + thetaLength, PI);
        int index = 0;

        widthSegments = glm::max(3.0f, glm::floor(widthSegments));
        heightSegments = glm::max(2.0f, glm::floor(heightSegments));

        vector<vector<int>> grid;

        glm::vec3 pos = glm::vec3(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);

        Vertex vertex;

        // 计算vertices normals 和 uvs
        for (unsigned int iy = 0; iy <= heightSegments; ++iy)
        {
            vector<int> verticesRow;
            float v = iy / heightSegments;

            float uOffset = 0;

            if (iy == 0 && thetaStart == 0)
            {
                uOffset = 0.5 / widthSegments;
            }
            else
            {
                uOffset = -0.5 / widthSegments;
            }
            for (unsigned int ix = 0; ix <= widthSegments; ++ix)
            {
                const float u = ix / widthSegments;

                pos.x = -radius * glm::cos(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength);
                pos.y = radius * glm::cos(thetaStart + v * thetaLength);
                pos.z = radius * glm::sin(phiStart + u * phiLength) * glm::sin(thetaStart + v * thetaLength);

                vertex.Position = pos;

                normal = glm::normalize(pos);
                vertex.Normal = normal;

                vertex.TexCoords = glm::vec2(u + uOffset, 1 - v);

                this->vertices.push_back(vertex);
                verticesRow.push_back(index++);
            }
            grid.push_back(verticesRow);
        }

        // indices
        for (unsigned int iy = 0; iy < heightSegments; ++iy)
        {
            for (unsigned int ix = 0; ix < widthSegments; ++ix)
            {
                unsigned int a = grid[iy][ix + 1];
                unsigned int b = grid[iy][ix];
                unsigned int c = grid[iy + 1][ix];
                unsigned int d = grid[iy + 1][ix + 1];

                if (iy != 0 || thetaStart > 0)
                {
                    this->indices.push_back(a);
                    this->indices.push_back(b);
                    this->indices.push_back(d);
                }
                if (iy != heightSegments - 1 || thetaEnd < PI)
                {
                    this->indices.push_back(b);
                    this->indices.push_back(c);
                    this->indices.push_back(d);
                }
            }
        }
        this->setupBuffers();
    }
};

#endif