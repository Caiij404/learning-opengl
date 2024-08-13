#version 330 core
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 BrightColor;
out vec4 FragColor;
in vec2 outTexCoord;

uniform sampler2D hdrBuffer;
uniform float exposure;
uniform bool hdr;

void main() {

    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrBuffer, outTexCoord).rgb;

    // reinhard
    // vec3 result = hdrColor / (hdrColor + vec3(1.0));
    // exposure
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    FragColor = vec4(result, 1.0);

    // Check whether fragment output is higher than threshold, if so output as brightness color
    // 检查片段输出是否高于阈值
    float brightColor = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightColor > 1.0) {
        BrightColor = vec4(FragColor.rgb, 1.0);
    }
}