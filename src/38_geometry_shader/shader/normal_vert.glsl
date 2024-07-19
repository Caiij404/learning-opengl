#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;

out VS_OUT{
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;

void main(){
    gl_Position = view * model * vec4(Position, 1.0);
    mat3 normalMat = mat3(transpose(inverse(view * model)));
    // vs_out.normal = normalize(normalMat * Normal);
    vs_out.normal = normalize(vec3(vec4(normalMat * Normal, 0.0)));
}