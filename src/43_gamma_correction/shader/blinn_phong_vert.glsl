#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tCoords;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoords;
out vec3 fragPos;
out vec3 vertexNormal;

uniform float uvScale;

void main() {
    fragPos = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1.0);
    vertexNormal = mat3(transpose(inverse(model))) * normal;
    texCoords = tCoords * uvScale;
}
