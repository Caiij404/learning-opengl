## 高级 GLSL

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

当（使用 glDrawElements）进行索引渲染的时候，这个变量会存储正在绘制顶点的当前索引。当（使用 glDrawArrays）不使用索引进行绘制的时候，这个变量会储存从渲染调用开始的已处理顶点数量。

### 片段着色器变量

**`gl_FragCoord`**

- x 和 y 分量是片段的窗口空间(Window-space)坐标，原点为窗口左下角。glViewport 接口可以设置窗口大小，例如 800x600，那 x 分量的范围就是[0,800]，y 分量的范围就是[0,600]
- z 分量对应片段的深度值
- 只读，不可修改

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

设置片段的深度值[0.0,1.0]，如果着色器没有写入值到 gl_FragDepth，它会自动取用 gl_FragCoord.z 的值

> 深度缓冲是在片段着色器（以及模板测试）运行之后在屏幕空间中运行的

> 现在大部分的 GPU 都提供一个叫做提前深度测试(Early Depth Testing)的硬件特性。提前深度测试允许深度测试在片段着色器之前运行。只要我们清楚一个片段永远不会是可见的（它在其他物体之后），我们就能提前丢弃这个片段。
>
> 片段着色器通常开销都是很大的，所以我们应该尽可能避免运行它们。当使用提前深度测试时，片段着色器的一个限制是你不能写入片段的深度值。如果一个片段着色器对它的深度值进行了写入，提前深度测试是不可能的。OpenGL 不能提前知道深度值。

> 如果在片段着色器中设置 gl_FragDepth 的值，那么 OpenGL 会禁用所有的**提前深度测试**，因为 OpenGL 无法在片段着色器运行之前得知片段将拥有的深度值，片段着色器可能会修改这个值

**OpenGL4.2 可以在片段着色器顶部重新声明**

`layout (depth_<condition>) out float gl_FragDepth;`

`condition`可以为下面的值：

| 条件        | 描述                                                         |
| :---------- | :----------------------------------------------------------- |
| `any`       | 默认值。提前深度测试是禁用的，你会损失很多性能               |
| `greater`   | 你只能让深度值比`gl_FragCoord.z`更大                         |
| `less`      | 你只能让深度值比`gl_FragCoord.z`更小                         |
| `unchanged` | 如果你要写入`gl_FragDepth`，你将只能写入`gl_FragCoord.z`的值 |

通过将深度条件设置为`greater`或者`less`，OpenGL 就能假设你只会写入比当前片段深度值更大或者更小的值了。这样子的话，当深度值比片段的深度值要小的时候，OpenGL 仍是能够进行提前深度测试的。

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

### uniform 块

```glsl
layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
}
```

uniform 块的变量可以直接访问，无需`Matrices`前缀，每个声明了这个 uniform 块的着色器都能访问。

### uniform 块布局

Uniform 块的内容是存储在一个缓冲对象中的，它实际是一块预留内存。因为这块内存不会保留它具体保存的数据是什么类型，因此我们还需要告诉 OpenGL 内存哪一部分对应着色器哪一个 uniform 变量。

假设有如此一个 uniform 块

```glsl
layout (std140) uniform ExampleBlock
{
	float fv;
	vec3 vt;
	mat4 matrix;
	float fVals[3];
	bool bVal;
	int iVal;
};
```

我们需要知道每个变量的大小（字节）和偏移量（从块起始位置），来让我们按顺序将它们放进缓冲中。每个元素的大小都是在 OpenGL 有清楚的声明，但没有声明这些变量之间的间距(Spacing)。比如`ExampleBlock`中，float 类型变量`fv`在前，而由 3 个 float 组成的 vec3 类型的`vt`在后，但硬件可能在附加 fv 之前，先将 vt 填充(pad)为一个 4 个 float 的数组。这个特性就让我们无法知晓偏移量。

在默认情况下，GLSL 会使用一个叫**共享(Shared)**布局的 Uniform 内存布局。共享是因为一旦硬件定义了偏移量，它们在多个程序中是**共享**并一致的。可以使用像`glGetUniformIndices`的函数来查询偏移量。

然而使用共享布局，就需要查询每个 uniform 变量的偏移量，增加了工作量。还有一种布局就是`std140`，它会显式声明每个变量类型的内存布局，让我们可以手动计算出每个变量的偏移量。

**基准对齐量**

表示一个变量在 uniform 块中占据的空间，比如`int`、`float`、`bool`都是 4 个字节。

每 4 个字节用`N`表示

| 类型                   | 布局规则                                                       |
| :--------------------- | :------------------------------------------------------------- |
| 标量，比如 int 和 bool | 每个标量的基准对齐量为 N。                                     |
| 向量                   | 2N 或者 4N。这意味着 vec3 的基准对齐量为 4N。                  |
| 标量或向量的数组       | 每个元素的基准对齐量与 vec4 的相同。                           |
| 矩阵                   | 储存为列向量的数组，每个向量的基准对齐量与 vec4 的相同。       |
| 结构体                 | 等于所有元素根据规则计算后的大小，但会填充到 vec4 大小的倍数。 |

**对齐偏移量**

对每个变量，我们再计算它的对齐偏移量(Aligned Offset)，它是一个变量从块起始位置的字节偏移量。一个变量的对齐字节偏移量必须等于基准对齐量的倍数。

```glsl
layout (std140) uniform ExampleBlock
{
                     // 基准对齐量       // 对齐偏移量
    float value;     // 4               // 0
    vec3 vector;     // 16              // 16  (必须是16的倍数，所以 4->16)
    mat4 matrix;     // 16              // 32  (列 0)
                     // 16              // 48  (列 1)
                     // 16              // 64  (列 2)
                     // 16              // 80  (列 3)
    float values[3]; // 16              // 96  (values[0])
                     // 16              // 112 (values[1])
                     // 16              // 128 (values[2])
    bool boolean;    // 4               // 144
    int integer;     // 4               // 148
};
```

### Uniform 缓冲

顶点着色器

```glsl
layout(std140) uniform Matrices {
    mat4 projection;
    mat4 view;
};
```

1. 创建缓冲对象

```c++
// 创建uniform缓冲对象
GLuint ubo;
glGenBuffers(1, &ubo);
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW); // 分配内存 data为NULL
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

2. 链接绑定点

```c++
// 给ubo对象的绑定点0的内存，声明大小
// 1.缓冲类型 2.绑定点 3.缓冲对象 4.偏移量 5.大小
glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
// 这接口也可以绑定
// glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
```

3. 获取 Uniform 块索引并设置绑定点

```c++
// 获取块索引
GLuint uniformBlockIndex_1 = glGetUniformBlockIndex(sceneShader1.ID, "Matrices");
GLuint uniformBlockIndex_2 = glGetUniformBlockIndex(sceneShader2.ID, "Matrices");
GLuint uniformBlockIndex_3 = glGetUniformBlockIndex(sceneShader3.ID, "Matrices");
GLuint uniformBlockIndex_4 = glGetUniformBlockIndex(sceneShader4.ID, "Matrices");

// 将顶点着色器中uniform块设置绑定点为0
glUniformBlockBinding(sceneShader1.ID, uniformBlockIndex_1, 0);
glUniformBlockBinding(sceneShader2.ID, uniformBlockIndex_2, 0);
glUniformBlockBinding(sceneShader3.ID, uniformBlockIndex_3, 0);
glUniformBlockBinding(sceneShader4.ID, uniformBlockIndex_4, 0);
```

4. 填充数据

```c++
// 循环外可以先填充好不变的投影矩阵
glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
// 1.缓冲类型 2.偏移量 3.数据大小 4.数据
glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
glBindBuffer(GL_UNIFORM_BUFFER, 0);

// 循环内再更新相机矩阵
glm::mat4 view = camera.GetViewMatrix();
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
glBindBuffer(GL_UNIFORM_BUFFER, 0);
```

**优点**

- 一次设置很多uniform会比一个一个设置多个uniform速度快
- 比起在多个着色器重修改同样的uniform，在Uniform缓冲中修改一次会更容易一些
- 使用Uniform缓冲对象，可以在着色器重使用更多的uniform，最大数
