#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCoord;

// 采样器 Sampler, 后缀1D/2D/3D是类型
uniform sampler2D texture1;
// 片段着色器也应该能使用采样器,因此在这里定义一个uniform的2D采样器
// 在main中,绑定纹理glBindTexture,就会自动把纹理赋值给片段着色器的采样器
uniform sampler2D texture2;

uniform vec3 lightColor;
uniform vec3 cubeColor;

void main() {
    vec4 color1 = vec4(lightColor * cubeColor, 1.0f);
    // FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
    vec4 color2 = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);
    FragColor = color1 * color2;
}