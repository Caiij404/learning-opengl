#version 330 core
layout(location = 0) in vec2 apos;
layout(location = 1) in vec3 acolor;

out VS_OUT {
    vec3 color;
} vs_out;

void main() {
    vs_out.color = acolor;
    gl_Position = vec4(apos, 0, 1.0);
}