#version 330 core
layout(location = 0) vec3 pos;
layout(location = 1) vec3 normal;
layout(location = 2) vec2 texCoords;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0);
    gl_PointSize = 10.0;
}