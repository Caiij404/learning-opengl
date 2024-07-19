#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tCoords;

// layout (std140) uniform mats{
//     mat4 projection;
//     mat4 view;
// };

uniform mat4 projection;
uniform mat4 view;

uniform mat4 model;

out VS_OUT{
    vec2 texCoords;
} vs_out;

void main(){
    vs_out.texCoords = tCoords;
    gl_Position = projection * view * model * vec4(position, 1.0);
}
