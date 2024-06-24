#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vertexNormal;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexNormal = mat3(transpose(inverse(model))) * aNormal;

    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoord = aTexCoord;
}

// 练习2 尝试在观察空间计算冯氏光照
// 其实很简单，现在是在世界空间计算的，所有的数据都是世界空间的向量，只要将用到的世界空间向量转成观察空间的即可
/* 
uniform vec3 lightPos;
out vec3 lightPosition;
...
{
    vertexNormal = mat3(transpose(inverse(view * model))) * aNormal;
    fragPos = vec3(view * model * vec4(aPos, 1.0));

    // 别忘了光源位置。既然要用到view和model两个矩阵，那光源位置就要从顶点着色器传到片段着色器了。
    lightPosition = vec3(view * model * vec4(lightPos, 1.0));
}
 */