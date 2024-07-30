#version 330 core
in vec4 FragPos;

uniform vec3 lightPosition;
uniform float far_plane;

void main() {
    // 获取片段和光源之间的距离
    float lightDist = length(FragPos.xyz - lightPosition);

    // 通过除以far_plane映射到[0,1]的范围
    lightDist = lightDist / far_plane;

    // 修改深度值
    gl_FragDepth = lightDist;
}