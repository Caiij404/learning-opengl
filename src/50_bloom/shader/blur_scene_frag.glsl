#version 330 core
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D image;

uniform bool horizontal;

// uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

uniform float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main(){
    // 计算单个像素的大小
    vec2 tex_offset = 1.0 / textureSize(image, 0);
    vec3 result = texture(image, outTexCoord).rgb * weight[0];

    if(horizontal)
    {
        for(int i=1; i<5; ++i)
        {
            result += texture(image, outTexCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(image, outTexCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
        }
    }
    else
    {
        for(int i=1; i<5; ++i)
        {
            result += texture(image, outTexCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(image, outTexCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
        }
    }

    FragColor = vec4(result, 1.0);

}