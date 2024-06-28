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
    vec4 position;
    // vec4 direction; //光线的方向，从原点指向光源位置

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Material material0;
uniform Light light0;

void main() {
    vec4 objectColor = vec4(1.0f);

    vec3 lightPos = vec3(light0.position);
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (light0.constant + light0.linear*distance + light0.quadratic*(distance * distance));
    
    // 环境光
    vec3 ambient = light0.ambient * vec3(texture(material0.diffuse, texCoord)) ;

    // 漫反射
    vec3 frag2Light = normalize(lightPos - fragPos);
    vec3 norm = normalize(vertexNormal);
    float diff = max(dot(frag2Light, norm), 0.0);
    vec3 diffuse = light0.diffuse * diff * vec3(texture(material0.diffuse, texCoord));

    // 镜面反射
    vec3 frag2view = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-frag2Light, norm);
    float spec = pow(max(dot(frag2view, reflectDir), 0.0), material0.shininess);
    vec3 tmp = vec3(texture(material0.specular, texCoord));
    vec3 specular = light0.specular * spec * tmp;

    // 乘以衰减系数
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = (ambient + diffuse + specular) * vec3(objectColor);

    FragColor = vec4(result, 1.0);
}
