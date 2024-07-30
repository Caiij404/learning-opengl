// 几何着色器负责将所有世界空间的顶点变换到6个不同的光空间着色器。
// 因此顶点着色器只需要把顶点变换为世界坐标即可
#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos;

void main() {
    for(int face = 0; face < 6; ++face) {
        // 当我们更新这个变量就能控制每个基本图形将渲染到立方体贴图的哪一个面
        // 内建变量，明确规定渲染哪个面
        gl_Layer = face;
        for(int i = 0; i < 3; ++i) {    // 遍历三角形顶点
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}