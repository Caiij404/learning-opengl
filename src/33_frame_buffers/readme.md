## 帧缓冲

### 创建帧缓冲

```c++
unsigned int fbo;
glGenFramebuffers(1, &fbo);
glBindFramebuffer(GL_FRAMEBUFFER, fbo);
```

一个完整的帧缓冲需要满足以下条件：

- 附加至少一个缓冲（颜色、深度、模板缓冲）
- 至少有一个颜色附件（Attachment）
- 所有附件都必须是完整的（保留了内存）
- 每个缓冲都应该有相同的样本数量（sample）

上述条件都满足后，可以通过下面的方式判断帧缓冲是否完整。

```c++
glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE
```

### 纹理附件

附件是一个内存位置，它能够作为帧缓冲的一个缓冲，可以将它想象为一个图像。

```c++
// 创建纹理附件
unsigned int texBuffer;
glGenTextures(1, &texBuffer);
glBindTexture(GL_TEXTURE_2D, texBuffer);
// 这里和生成纹理的区别有几点：
// 1.将维度设置为屏幕大小 2.最后一个纹理data参数传NULL。
// 这个操作，仅仅给纹理分配了内存，而没有填充它。
// 填充纹理将会在渲染到帧缓冲之后进行
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

**将颜色纹理附加到当前绑定的帧缓冲上**

```c++
// 1.帧缓冲的目标（绘制、读取或二者都有） 2.想要附加的附件类型 3.希望附加的纹理类型 4.纹理 5.多级渐远纹理级别
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texBuffer, 0);
```

**将深度和模板缓冲附加为一个纹理到帧缓冲中**

```c++
// 纹理每32位数值将包含24位深度信息和8位模板信息。
// 将深度和模板缓冲附加为一个纹理
glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texBuffer, 0);
```

### 设置渲染缓冲对象

```c++
// 渲染缓冲对象
unsigned int rbo;
glGenRenderbuffers(1, &rbo);
// 绑定渲染缓冲对象，之后所有渲染缓冲操作将影响当前的rbo
glBindRenderbuffer(GL_RENDERBUFFER, rbo);
// 创建一个深度和模板渲染缓冲对象
glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
glBindRenderbuffer(GL_RENDERBUFFER, 0);
// 将渲染缓冲对象附加到帧缓冲的深度和模板附件上
glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
{
    std::cout<<"ERROR::Framebuffer is not complete!" << std::endl;
}
glBindFramebuffer(GL_FRAMEBUFFER, 0);
```
