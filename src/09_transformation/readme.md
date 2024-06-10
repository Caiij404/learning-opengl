## 知识点回顾

-   glm库矩阵的用法：
    *   矩阵变换方法是**右乘**，`translate(mat, vec3)`或`scale(mat, vec3)`或`rotate(mat, vec3)`等，都是**vec3转换mat1**后，`mat * mat1`。试验方式很简单：先位移后缩放，和先缩放后位移，看看结果。
    *   