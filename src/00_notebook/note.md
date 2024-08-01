## 易错点

1. 环绕和过滤

在写点光源阴影的那一部分时，直接用了定向光阴影的代码，直接就把深度贴图（阴影贴图）**设置环绕和过滤方式**抄过来，导致第一个参数没从`GL_TEXTURE_2D`改成`GL_TEXTURE_CUBE_MAP`，结果就是点光源阴影死活出不来。

```c++
// 这个参数写成2D的，一直出不来阴影，找了半天，倒是找出了好几处其他错误.....
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
```

`45_point_shadow.cpp`

<br>

2. 纹理单元

在main.cpp中加载了几个纹理，并用在同一个着色器中，那就不要忘了把纹理和纹理单元对应上。

```c++
// 通知着色器
ourShader.setInt("map1", 0);
ourShader.setInt("map2", 1);

// 激活纹理单元并绑定纹理
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, floorMap);

glActiveTexture(GL_TEXTURE1);
glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

// 更换某个纹理
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, brickMap);
```

每个步骤都不可少，不然要么纹理出不来，要么纹理贴错。像`44_shadow_mapping`的`readme.md`记录中，阴影纹理虽然出现了，但贴不对。

<br>

3. `dispose`

程序结束，不要忘记解绑并删除缓冲。在调试`45_point_shadow`问题时，因为漏了`dispose()`，并且短时间内多次`make run`，然后程序就变得莫名奇妙。

就比如：

```c++
sceneShader.use();
sceneShader.setFloat("uvScale", 4.0f);

// ······  在此之间没有drawMesh

sceneShader.setFloat("uvScale", 1.0f);
drawMesh(geometry);
```

然后在第一次drawMesh之前有两个`setFloat("uvScale")`，明显有一个多余的，但我把第一个注释掉，tm的就是跑不动，总是报出一串很大的数字的错误，应该就是内存泄漏了。