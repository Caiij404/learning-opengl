#version 330 core
layout(location = 0) in vec3 position;

uniform mat4 lightSpaceMat;
uniform mat4 model = mat4(1.0);

void main() {
    gl_Position = lightSpaceMat * model * vec4(position, 1.0);
}