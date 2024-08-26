## stencil test

当片段着色器运行完，会进行模板测试和深度测试，二者都能决定片段的保留或丢弃(discard)，因此只有通过模板测试的片段才会进入深度测试。


模板测试是根据一个模板缓冲(Stencil Buffer)进行的，GLFW会给每个窗口默认配置一个模板缓冲，因此无需手动创建buffer。

**开启/关闭模板缓冲**

```cpp
glEnable(GL_STENCIL_TEST);
glDisable(GL_STENCIL_TEST);
```

渲染的每个循环开头，都应该先**清除上一循环留下的模板缓冲**

```cpp
glClear(GL_STENCIL_BUFFER_BIT);
```

模板缓冲允许**读取**和**写入**，但在**写入**之前会**先与设定好的掩码Mask进行与(AND)运算**

```cpp
glStencilMask(0xFF);    // 写入模板缓冲时保持原样
glStencilMask(0x00);    // 写入的都变成0，和深度测试的glDepthMask(GL_FALSE)等价
```


### 模板函数



[可参考知乎答案](https://zhuanlan.zhihu.com/p/612811622)