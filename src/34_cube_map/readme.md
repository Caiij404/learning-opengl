## 立方体贴图

### 天空盒

| 纹理目标                         | 方位 | 简称 |
| :------------------------------- | :--- | :--- |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_X` | 右   | px   |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_X` | 左   | nx   |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_Y` | 上   | py   |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_Y` | 下   | ny   |
| `GL_TEXTURE_CUBE_MAP_POSITIVE_Z` | 后   | pz   |
| `GL_TEXTURE_CUBE_MAP_NEGATIVE_Z` | 前   | nz   |

**可以看`BoxGeometry.h`的构造函数，生成面的顺序也是按从`positive`到`negative`，从`x`到`y`到`z`**

![](img/1.png)

### 绘制天空盒

**vertex glsl**

```glsl
#version 330 core
layout(location = 0) in vec3 Position;
out vec3 outTexCoord;
uniform mat4 view;
uniform mat4 projection;

void main() {
	outTexCoord = Position;
	vec4 pos = projection * view * vec4(Position, 1.0);
	gl_Position = pos;
}
```

**fragment shader**

```glsl
#version 330 core
out vec4 FragColor;
in vec3 outTexCoord;

uniform samplerCube skyboxTexture;

void main() {
    FragColor = texture(skyboxTexture, outTexCoord);
}
```

```c++
// 绘制天空盒
glDepthFunc(GL_LEQUAL);
glDisable(GL_DEPTH_TEST);

skyboxShader.use();
skyboxShader.setMat4("view", view);
skyboxShader.setMat4("projection", projection);

glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
glBindVertexArray(skyboxGeometry.VAO);
glDrawElements(GL_TRIANGLES, skyboxGeometry.indices.size(), GL_UNSIGNED_INT, 0);

glBindVertexArray(0);
glDepthFunc(GL_LESS);
glEnable(GL_DEPTH_TEST);
```

![](img/2.png)

### 移除平移

```c++
view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // 移除平移分量

// 绘制其它物体时恢复
view = camera.GetViewMatrix();
```

![](img/3.png)


## 参考

https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/

<br>
<br>
<br>


-----

<br>
<br>
<br>

遗留一个问题：在做本章节的优化小节时，提到可以把skybox放在最后绘制而无需关闭深度测试，但发现有alpha的纹理，比如窗户没法透过看到天空，草贴图完全透明和完全不透明的边界，会和天空有奇怪的像素点等问题

![](img/4.png)