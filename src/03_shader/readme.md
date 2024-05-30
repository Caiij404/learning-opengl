## 知识点回顾

-   GLSL, openGL Shader Language, 着色器语言
-   GLSL的数据类型包括C等其他语言大部分的默认基础数据类型: int, float, double, uint, bool; 以及两种容器Vector 和 Matrix
    *   vecn，包含n个float分量的向量。 b\i\u\dvecn 分别就是bool\int\uint\double类型的。
    *   向量的构建很灵活，又称为重组(Swizzling)
-   输入与输出
    *   两个关键字 in 和 out
    *   接收输入