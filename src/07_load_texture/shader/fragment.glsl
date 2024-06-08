#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 texCoord;

// 采样器 Sampler, 后缀1D/2D/3D是类型
uniform sampler2D texture1;
// 片段着色器也应该能使用采样器,因此在这里定义一个uniform的2D采样器
// 在main中,绑定纹理glBindTexture,就会自动把纹理赋值给片段着色器的采样器
uniform sampler2D texture2;

void main() {
    // FragColor = texture(texture1, texCoord) /* * vec4(ourColor, 1) */;
    FragColor = mix(texture(texture1, texCoord), texture(texture2, texCoord), 0.2);

}