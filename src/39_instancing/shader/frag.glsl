#version 330 core
out vec4 FragColor;

in vec2 oTexCoord;

void main()
{
    FragColor = vec4(vec3(oTexCoord, 1.0), 1.0);
}