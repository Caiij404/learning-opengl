## HDR

high dynamic range 高动态范围

在多光源影响下，场景中某范围的片段颜色值和可能超过 1.0，但又会被约束在 1.0 内，就会导致该范围细节丢失。

而 HDR 可以使颜色值和大于 1.0，让亮的片段更亮，暗的片段更暗，且充满细节。

## 浮点帧缓冲 Floating Point Framebuffer

帧缓冲使用标准化的定点格式（`GL_RGB`）为其颜色缓冲的内部格式，OpenGL 会将这些值存入帧缓冲前自动约束（clamp）到[0.0, 1.0]之间。

当颜色缓冲的内部格式被设定为`GL_RGB16F`，`GL_RGBA16F`，`GL_RGB32F`或`GL_RGBA32F`时，这些帧缓冲被叫做浮点帧缓冲。

```c++
glBindTexture(GL_TEXTURE_2D, colorBuffer);
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
```

### 色调映射 Tone Mapping

允许颜色值超过 1.0，将 HDR 值转换为范围在[0.0, 1.0]的 LDR（Low Dynamic Range 低动态范围）

**hdr_quad_vert.glsl**

```glsl
#version 330 core
layout(location = 0) in vec3 Position;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

out vec2 outTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    // gl_Position = projection * view * model * vec4(Position, 1.0f);
    gl_Position = model * vec4(Position, 1.0);
    outTexCoord = TexCoords;
}
```

**hdr_quad_frag.glsl**

```glsl
#version 330 core
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
}
```