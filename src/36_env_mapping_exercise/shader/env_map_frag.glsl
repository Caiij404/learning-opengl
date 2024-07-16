#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;
in vec2 tCoords;

uniform vec3 cameraPos;
uniform sampler2D texture_height1;
uniform samplerCube skybox;

void main() {
    // 反射
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    // 用R作为索引，采样立方体贴图
    vec4 envColor = texture(skybox, R);

    FragColor = texture(texture_height1, tCoords) * envColor;

}