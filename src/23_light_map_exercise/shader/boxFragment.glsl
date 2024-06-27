#version 330 core
out vec4 FragColor;

in vec3 vertexNormal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 viewPos;

struct Material {
    sampler2D ambient;  // 环境光贴图
    sampler2D emission; // 发光贴图
    sampler2D diffuse;  // 漫反射贴图
    sampler2D specularColor; // 镜面光彩色贴图
    sampler2D specular; // 镜面贴图

    float shininess;    // 反光度
};

// 光照属性
struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material0;
uniform Light light0;
uniform bool flag = false;
uniform float factor = 0.0f;

void main() {
    vec4 objectColor = vec4(1.0f);
    // 环境光
    // vec3 ambient = material0.ambient * light0.ambient;
    vec3 ambient = light0.ambient * vec3(texture(material0.diffuse, texCoord));

    // 漫反射
    vec3 frag2light = normalize(light0.position - fragPos);
    vec3 norm = normalize(vertexNormal);
    float diff = max(dot(frag2light, norm), 0.0);
    // vec3 diffuse = (diff * material0.diffuse) * light0.diffuse;
    vec3 diffuse = light0.diffuse * diff * vec3(texture(material0.diffuse, texCoord));

    // 镜面反射
    vec3 frag2view = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-frag2light, norm);
    float spec = pow(max(dot(frag2view, reflectDir), 0.0), material0.shininess);
    // vec3 specular = (spec * material0.specular) * light0.specular;
    vec3 tmp = vec3(1.0, 1.0, 1.0);
    if(flag) {
        // 练习2
        // tmp = vec3(texture(material0.specular, texCoord));
        // tmp = vec3(1.0 - tmp.x, 1.0 - tmp.y, 1.0 - tmp.z);
        tmp = tmp - vec3(texture(material0.specularColor, texCoord));
    }
    else
    {
        tmp = vec3(texture(material0.specular, texCoord));
    }
    vec3 specular = light0.specular * spec * tmp;

    // 发光贴图
    vec2 uv = texCoord;
    uv.y += factor;
    vec3 emissionTexture = vec3(texture(material0.emission, uv));

    // vec3 result = (ambient + diffuse + specular) * objColor;
    vec3 result = (ambient + diffuse + specular + emissionTexture) * vec3(objectColor);

    FragColor = vec4(result, 1.0);
}
