#version 330 core
layout(location = 0) in vec2 Position;
// layout(location = 1) in vec3 Normal;
layout(location = 1) in vec2 TexCoords;

out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // gl_Position = projection * view * model * vec4(Position, 1.0f);
    gl_Position = vec4(Position,0.0, 1.0);
    outTexCoord = TexCoords;
}