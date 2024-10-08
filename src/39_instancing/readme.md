## 实例化

设置实例数据

```c++
glm::vec3 translations[100];
int index = 0;
float offset = 0.3f;
glm::vec3 tran;
for (int y = -10; y < 10; y += 2)
{
    for (int x = -10; x < 10; x += 2)
    {
        tran.x = (float)x / 10.0f + offset;
        tran.y = (float)y / 10.0f + offset;
        tran.z = 0.0;
        translations[index++] = tran;
    }
}
sceneShader.use();
for(int i=0; i<100; ++i)
{
    sceneShader.setVec3("offsets[" + std::to_string(i) + "]", translations[i]);
}
```

**vertex shader**

```glsl
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec2 oTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 offsets[100];

void main(){
    vec3 offset = offsets[gl_InstanceID];
    gl_Position = projection * view * model * vec4(aPos + offset, 1.0);
    oTexCoord = texCoords;
}
```

**render**

```c++
sceneShader.use();
sceneShader.setMat4("projection", projection);
sceneShader.setMat4("view", view);
sceneShader.setMat4("model", model);

glBindVertexArray(boxGeometry.vao);
// 索引方式绘制
// mode 图元类型
// count 索引数组的长度
// type 索引值类型
// indices 绘制起始量
// primcount 指定要呈现的索引范围的实例数
glDrawElementsInstanced(GL_TRIANGLES, boxGeometry.indices.size(), GL_UNSIGNED_INT, 0, 100);
glBindVertexArray(0);
```

![](img/1.png)


![](img/2.png)

## 实例化数组

使用`uniform vec3 offsets[100];`这种方式渲染数量较小的时候比较方便，但是如果大批量数据的话，就会很快超过能够发送至着色器`uniform`的上限，替代化方案是实例化数组。

它被定义为一个顶点属性，仅在顶点着色器渲染一个新的实例时才会更新。

使用顶点属性时，顶点着色器每次运行都会让GLSL获取新一组适用于当前顶点的属性，而当我将顶点属性定义为一个实例数组时，顶点着色器就只需要对每个实例，而不是每个顶点，去更新顶点属性

- 对逐顶点的数据使用普通的顶点属性
- 对逐实例的数据使用实例化数组
