#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 texCoord;

// 注意要初始化！！！！
// 不然如果不传入矩阵，gl_Position就失效了
uniform mat4 mat = mat4(1.0f);

void main() {
    gl_Position = mat * vec4(aPos, 1.0);
    // ourColor = aColor;
    // texCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
    texCoord = aTexCoord;
}