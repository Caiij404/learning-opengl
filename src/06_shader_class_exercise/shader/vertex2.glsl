// 练习1
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 ourColor;

void main() {
    vec3 tmp = aPos;
    tmp.y *= -1;
    gl_Position = vec4(tmp, 1.0);
    ourColor = aColor;
}