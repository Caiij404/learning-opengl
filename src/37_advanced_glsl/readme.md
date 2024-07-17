## 高级GLSL

### 内建变量 Built-in Variable

目前接触到的内建变量有：`gl_Position`和`gl_FragCoord`

### 顶点着色器变量

**`gl_PointSize`**

在顶点着色器中，设置`gl_Position`是必不可少的一步，他是顶点着色器的**裁剪空间**输出位置向量。

而如果选用的图元是`GL_POINTS`，即一个点，那还可以通过设置**float**类型的变量`gl_PointSize`设置该**点图元**的宽高(像素)。

但要手动开启该功能:
```c++
glEnable(GL_PROGRAM_POINT_SIZE);
```

**`gl_VertexID`** ：输入变量，存储正在绘制顶点的当前索引

当（使用glDrawElements）进行索引渲染的时候，这个变量会存储正在绘制顶点的当前索引。当（使用glDrawArrays）不使用索引进行绘制的时候，这个变量会储存从渲染调用开始的已处理顶点数量。

### 片段着色器变量

**`gl_FragCoord`**

-   x和y分量是片段的窗口空间(Window-space)坐标，原点为窗口左下角。glViewport接口可以设置窗口大小，例如800x600，那x分量的范围就是[0,800]，y分量的范围就是[0,600]
-   z分量对应片段的深度值
-   只读，不可修改


**将屏幕分成不同颜色**

```glsl
  vec3 result = vec3(1.0);
  if(gl_FragCoord.x < 400) {
    result = vec3(1.0, 0.0, 0.0);
  } else {
    result = vec3(1.0, 0.0, 1.0);
  }
```

**`gl_FrontFacing`**

`bool`类型变量，如果该片段是正向面的一部分则为`true`，否则`false`

通过此变量显示内外显示不同的纹理

```glsl
  vec3 result = vec3(1.0);
  if(gl_FrontFacing) {
    result = texture(uvMap, oTexCoord).rgb;
  } else {
    result = texture(triMap, oTexCoord).rgb;
  }
  FragColor = vec4(result, 1.0);
```

### gl_FragDepth

设置片段的深度值[0.0,1.0]，如果着色器没有写入值到gl_FragDepth，它会自动取用gl_FragCoord.z的值

> 深度缓冲是在片段着色器（以及模板测试）运行之后在屏幕空间中运行的

> 现在大部分的GPU都提供一个叫做提前深度测试(Early Depth Testing)的硬件特性。提前深度测试允许深度测试在片段着色器之前运行。只要我们清楚一个片段永远不会是可见的（它在其他物体之后），我们就能提前丢弃这个片段。
>
> 片段着色器通常开销都是很大的，所以我们应该尽可能避免运行它们。当使用提前深度测试时，片段着色器的一个限制是你不能写入片段的深度值。如果一个片段着色器对它的深度值进行了写入，提前深度测试是不可能的。OpenGL不能提前知道深度值。

> 如果在片段着色器中设置gl_FragDepth的值，那么OpenGL会禁用所有的**提前深度测试**，因为OpenGL无法在片段着色器运行之前得知片段将拥有的深度值，片段着色器可能会修改这个值


**OpenGL4.2可以在片段着色器顶部重新声明**

`layout (depth_<condition>) out float gl_FragDepth;`

`condition`可以为下面的值：

| 条件        | 描述                                                         |
| :---------- | :----------------------------------------------------------- |
| `any`       | 默认值。提前深度测试是禁用的，你会损失很多性能               |
| `greater`   | 你只能让深度值比`gl_FragCoord.z`更大                         |
| `less`      | 你只能让深度值比`gl_FragCoord.z`更小                         |
| `unchanged` | 如果你要写入`gl_FragDepth`，你将只能写入`gl_FragCoord.z`的值 |

通过将深度条件设置为`greater`或者`less`，OpenGL就能假设你只会写入比当前片段深度值更大或者更小的值了。这样子的话，当深度值比片段的深度值要小的时候，OpenGL仍是能够进行提前深度测试的。

```glsl
#version 420 core // 注意GLSL的版本！
out vec4 FragColor;
layout (depth_greater) out float gl_FragDepth;

void main()
{             
    FragColor = vec4(1.0);
    gl_FragDepth = gl_FragCoord.z + 0.1;
}
```

### 接口块

顶点着色器到片段着色器的输入

**vertex shader**

```glsl
out VS_OUT
{
    vec2 TexCoords;
} vs_out;
```

**fragment shader**

```glsl
in VS_OUT
{
    vec2 TexCoords;
} fs_in;
```

### uniform块

```glsl
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
}
```

uniform块的变量可以直接访问，无需`Matrices`前缀，每个声明了这个uniform块的着色器都能访问。