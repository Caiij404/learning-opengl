## 知识点回顾

冯氏光照模型 Phong Lighting Model，主要结构由3个分量组成：环境(Ambient)、漫反射(Diffuse)和镜面(Specular)光照。

-   环境光照(Ambient Lighting)：即使在黑暗的情况下，世界上通常也仍然有一些光亮（月亮、远处的光），所以物体几乎永远不会是完全黑暗的。为了模拟这个，我们会使用一个环境光照常量，它永远会给物体一些颜色。
-   漫反射光照(Diffuse Lighting)：模拟光源对物体的方向性影响(Directional Impact)。它是冯氏光照模型中视觉上最显著的分量。物体的某一部分越是正对着光源，它就会越亮。
-   镜面光照(Specular Lighting)：模拟有光泽物体上面出现的亮点。镜面光照的颜色相比于物体的颜色会更倾向于光的颜色。


### 环境光

```glsl
void main()
{
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * objectColor;
    FragColor = vec4(result, 1.0);
}
```

### 漫反射

```glsl
#version 330 core
out vec4 FragColor;
in vec3 outNormal;//法向量
in vec3 outFragPos; // 片元位置

uniform vec3 lightPos;//光照位置
uniform vec3 viewPos; // 视线方向

void main()
{
    // 环境光常量
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
		
    vec3 norm = normalize(outNormal); // 法向量单位化
    vec3 lightDir = normalize(lightPos - outFragPos); // 片元位置指向光照方向

    float diff = max(dot(norm, lightDir), 0.0); // 漫反射分量
    vec3 diffuse = diff * lightColor; // 漫反射
	
    vec3 result = (ambient + diffuse) * objectColor;
    
    FragColor = vec4(result, 1.0);
}
```

#### 法向量

法向量从顶点着色器传出，在片段着色器传入并使用。法向量一般都是局部坐标系的，需要使用**模型矩阵**转换到世界坐标系的。
`vertexNormal = mat3(transpose(inverse(model))) * aNormal;`
```glsl
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 vertexNormal;
out vec3 fragPos;

uniform mat4 projection = mat4(1.0);
uniform mat4 view = mat4(1.0);
uniform mat4 model = mat4(1.0);

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexNormal = mat3(transpose(inverse(model))) * aNormal;

    fragPos = vec3(model * vec4(aPos, 1.0));
    
}
```



### 镜面光

```glsl
#version 330 core
out vec4 FragColor;
in vec3 outNormal;//法向量
in vec3 outFragPos; // 片元位置

uniform vec3 lightPos;//光照位置
uniform vec3 viewPos; // 视线方向

void main()
{
    // 环境光常量
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
		
    vec3 norm = normalize(outNormal); // 法向量单位化
    vec3 lightDir = normalize(lightPos - outFragPos); // 片元位置指向光照方向

    float diff = max(dot(norm, lightDir), 0.0); // 漫反射分量
    vec3 diffuse = diff * lightColor; // 漫反射
    
    float specularStrength = 0.9;
    vec3 viewDir = normalize(viewPos - outFragPos); // 视线方向
    vec3 reflectDir = reflect(-lightDir, norm); // 沿法向量反射

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor; // 镜面光

	
    vec3 result = (ambient + diffuse + specular) * objectColor;
    
    FragColor = vec4(result, 1.0);
}
```


## 参考

https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/#_3
