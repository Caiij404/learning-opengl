## 绘制小行星带

### 实例化数组

使用`uniform vec3 offsets[100];`这种方式渲染数量较小的时候比较方便，但是如果大批量数据的话，就会很快超过能够发送至着色器`uniform`的上限，替代化方案是实例化数组。

它被定义为一个顶点属性，**仅在顶点着色器渲染一个新的实例时才会更新**。

使用顶点属性时，顶点着色器每次运行都会让GLSL获取新一组适用于当前顶点的属性，而将顶点属性定义为一个实例数组时，顶点着色器就只需要对每个实例，而不是每个顶点，去更新顶点属性。

- 对逐顶点的数据使用普通的顶点属性
- 对逐实例的数据使用实例化数组


**实例数组渲染方式**

**设置实例化数组**

```c++
unsigned int buffer;
glGenBuffers(1, &buffer);
glBindBuffer(GL_ARRAY_BUFFER, buffer);
glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
for (unsigned int i = 0; i < rock.meshes.size(); ++i)
{
    unsigned int vao = rock.meshes[i].vao;
    glBindVertexArray(vao);
    // 顶点属性
    GLsizei vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void *)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glBindVertexArray(0);
}
```

**渲染**

```c++
instanceShader.use();
instanceShader.setMat4("projection", projection);
instanceShader.setMat4("view", view);
instanceShader.setInt("diffuseTexture", 0);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, rock.textures_loaded[0].id);
for (int i = 0; i < rock.meshes.size(); ++i)
{
    glBindVertexArray(rock.meshes[i].vao);
    glDrawElementsInstanced(GL_TRIANGLES, rock.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
}
```

**vertex shader**

```glsl
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

layout(location = 3) in mat4 instanceMatrix;

out vec2 oTexCoord;

uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * instanceMatrix * vec4(aPos, 1.0);
    oTexCoord = texCoords;
}
```