#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform float uvScale = 1.0;
uniform int reverse_normal = 1;

void main() {
    vs_out.FragPos = vec3(model * vec4(Position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * (reverse_normal * Normal);
    vs_out.TexCoords = TexCoords * uvScale;
    gl_Position = projection * view * model * vec4(Position, 1.0);
}