## 回顾

### [光的属性](https://learnopengl-cn.github.io/02%20Lighting/03%20Materials/#_3)

这一节中，描述有点抽象了，差点没看明白。

_光源对环境光、漫反射和镜面光分量也分别具有不同的强度。_ —— 这句话，可以理解成环境光、漫反射光、镜面光三个分量组成**光源**，而分量各有各的强度。

_前面的章节中，我们通过使用一个强度值改变环境光和镜面光强度的方式解决了这个问题。_ —— 也就是`19_base_lighting`章节中片段着色器`ambientStrength`、`diff`、`specularStrength`三个**浮点值**。

_我们想做类似的事情，但是这次是要为每个光照分量分别指定一个**强度向量**。_ —— 注意最后说的是**强度向量**，我理解的意思是给三个光分量，设定不同的**向量**作为`rgb`。

_如果我们假设lightColor是`vec3(1.0)`，代码会看起来像这样：_

```
vec3 ambient  = vec3(1.0) * material.ambient;
vec3 diffuse  = vec3(1.0) * (diff * material.diffuse);
vec3 specular = vec3(1.0) * (spec * material.specular);
```

_所以物体的每个材质属性对每一个光照分量都返回了<span style="color: red;">最大的强度</span>_。 —— 这里说的最大的强度，就是`vec(1.0)`。

_对单个光源来说，这些`vec3(1.0)`值同样可以对每种光源分别改变，**而这通常就是我们想要的**。现在，物体的环境光分量完全地影响了立方体的颜色，可是环境光分量实际上不应该对最终的颜色有这么大的影响，所以我们会将光源的环境光强度设置为一个小一点的值，从而限制环境光颜色：_

`vec3 ambient = vec3(0.1) * material.ambient;`

<br>

### 材质
在上一节中，我们定义了一个物体和光的颜色，并结合环境光与镜面强度分量，来决定物体的视觉输出。当描述一个表面时，我们可以分别为三个光照分量定义一个材质颜色`(Material Color)`：环境光照`(Ambient Lighting)`、漫反射光照`(Diffuse Lighting)`和镜面光照`(Specular Lighting)`。通过为**每个分量指定一个颜色**，我们就能够对表面的颜色输出有细粒度的控制了。现在，我们再添加一个反光度`(Shininess)`分量，结合上述的三个颜色，我们就有了全部所需的材质属性了：
```
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;   // 反光度
}; 
```

<br>

### 光照属性

```
struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
}
```

-   环境光通常设置为比较低的强度，因为不希望环境光颜色太过主导。
-   光源的漫反射分量通常设置为希望光所具有的颜色，也就是比较明亮的白色。
-   镜面光分量通常保持为`vec3(1.0f)`，以最大强度发光。


最终在片段着色器中，可能如下：
```
uniform Light light;
uniform Material material;

// light.ambient 是环境光的强度
// material.ambient 是环境光在物体上反射的颜色
// 但程序中物体没颜色，这里是不是可以理解成光的颜色，虽然二者都是vec3类型变量，但意义不同
vec3 ambient = light.ambient * material.ambient;
vec3 diffuse = light.diffuse * (diff * material.diffuse);
vec3 specular = light.specular * (spec * material.specular);
```