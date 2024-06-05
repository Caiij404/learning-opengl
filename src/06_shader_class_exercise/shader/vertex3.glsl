// 练习2
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;
uniform float xOffset = 0;
uniform float yOffset = 0;
void main() {
    vec3 tmp = aPos;
    tmp.x += xOffset;
    tmp.y += yOffset;
    gl_Position = vec4(tmp, 1.0);
    ourColor = aColor;
}