#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 Position;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;

void main() {
    Normal = mat3(transpose(inverse(model))) * norm;
    Position = vec3(model * vec4(pos, 1.0));
    gl_Position = projection * view * vec4(Position, 1.0);
}