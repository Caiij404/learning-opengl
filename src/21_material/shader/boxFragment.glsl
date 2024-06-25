#version 330 core
out vec4 FragColor;

in vec3 vertexNormal;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform vec3 viewPos;

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

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
    // float ambientStrength = 0.1;
    // vec3 ambient = ambientStrength * lightColor;

    vec3 ambient = material0.ambient * light0.ambient;

    // 漫反射
    // vec3 frag2light = normalize(lightPos - fragPos);
    vec3 frag2light = normalize(light0.position - fragPos);
    vec3 norm = normalize(vertexNormal);
    float diff = max(dot(frag2light, norm), 0.0);
    // vec3 diffuse = diff * lightColor;
    vec3 diffuse = (diff * material0.diffuse) * light0.diffuse;

    // 镜面反射
    // float specularStrength = 0.5;
    vec3 frag2view = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-frag2light, norm);
    float spec = pow(max(dot(frag2view, reflectDir), 0.0), material0.shininess);
    // vec3 specular = specularStrength * spec * lightColor;
    vec3 specular = (spec * material0.specular) * light0.specular;


    // vec3 result = (ambient + diffuse + specular) * objColor;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}
