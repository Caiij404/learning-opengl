#version 330 core
out vec4 FragColor;

in vec3 vertexNormal;
in vec3 fragPos;
in vec2 texCoord;

uniform vec3 viewPos;

struct Material{
    sampler2D diffuse;
    // vec3 specular;
    sampler2D specular;

    float shininess;    // 反光度
};

// 光照属性
struct Light{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material0;
uniform Light light0;

void main() {
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
    vec3 specular = light0.specular * spec * vec3(texture(material0.specular, texCoord));

    // vec3 result = (ambient + diffuse + specular) * objColor;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
