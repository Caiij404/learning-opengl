// 练习1 
#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    vec2 texC = texCoord;
    texC.x *= -1;
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texC), 0.2);

}