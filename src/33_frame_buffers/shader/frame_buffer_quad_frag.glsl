#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform int renderType = 0;

const float offset = 1.0 / 300.0;
vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // 左上
    vec2( 0.0f,    offset), // 正上
    vec2( offset,  offset), // 右上
    vec2(-offset,  0.0f),   // 左
    vec2( 0.0f,    0.0f),   // 中
    vec2( offset,  0.0f),   // 右
    vec2(-offset, -offset), // 左下
    vec2( 0.0f,   -offset), // 正下
    vec2( offset, -offset)  // 右下
);

void main() {

    switch(renderType)
    {
        case 1: 
        {
            // 反相
            vec3 col = 1.0 - texture(screenTexture, TexCoords).rgb;
            FragColor = vec4(col, 1.0);
            break;
        }
        case 2:
        {
            // 灰度
            // 简单点就直接平均3个颜色分量，精确点就使用加权(weighted)通道
            FragColor = texture(screenTexture, TexCoords);
            float alpha = FragColor.a;
            float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
            FragColor = vec4(average, average, average, alpha);
            break;
        }
        case 3: 
        {
            // 核效果
            // 锐化(Sharpen)核
            float kernel[9] = float[](
                -1, -1, -1,
                -1,  9, -1,
                -1, -1, -1
            );

            vec3 sampleTex[9];
            for(int i = 0; i < 9; ++i)
            {
                sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
            }
            vec3 col = vec3(0.0);
            for(int i = 0; i < 9; ++i)
            {
                col += sampleTex[i] * kernel[i];
            }
            FragColor = vec4(col, 1.0);
            break;
        }
        case 5:
        {
            float kernel[9] = float[](
                1.0 / 16, 2.0 / 16, 1.0 / 16,
                2.0 / 16, 4.0 / 16, 2.0 / 16,
                1.0 / 16, 2.0 / 16, 1.0 / 16  
            );

            vec3 sampleTex[9];
            for(int i = 0; i < 9; ++i)
            {
                sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
            }
            vec3 col = vec3(0.0);
            for(int i = 0; i < 9; ++i)
            {
                col += sampleTex[i] * kernel[i];
            }
            FragColor = vec4(col, 1.0);
            break;
        }
        case 6:
        {
            float kernel[9] = float[](
                1, 1, 1,
                1, -8, 1,
                1, 1, 1
            );

            vec3 sampleTex[9];
            for(int i = 0; i < 9; ++i)
            {
                sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
            }
            vec3 col = vec3(0.0);
            for(int i = 0; i < 9; ++i)
            {
                col += sampleTex[i] * kernel[i];
            }
            FragColor = vec4(col, 1.0);
            break;
        }
        default:
        {
            // 正常渲染
            vec3 col = texture(screenTexture, TexCoords).rgb;
            FragColor = vec4(col, 1.0);
            break;
        }
    }
}