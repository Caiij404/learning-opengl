#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 aTexCoords;

out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

  outTexCoord = aTexCoords;
  gl_Position = vec4(Position, 1.0);

}