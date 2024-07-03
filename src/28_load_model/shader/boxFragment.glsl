#version 330 core
out vec4 FragColor;

in vec3 vertexNormal;
in vec3 fragPos;
in vec2 texCoord;

in vec3 viewCoord;

uniform vec3 viewPos;

struct Material {
    sampler2D ambient;  // 环境光贴图
    sampler2D emission; // 发光贴图
    sampler2D diffuse;  // 漫反射贴图
    sampler2D specularColor; // 镜面光彩色贴图
    sampler2D specular; // 镜面贴图

    sampler2D spotLightMap; // 聚光灯spotLight贴图

    float shininess;    // 反光度
};
uniform Material material0;

// 定向光源
struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dLight;

/**
*   计算定向光
*   @param light 定向光信息
*   @param nVt 顶点法线
*   @param viewDir fragment到相机方向
*/
vec3 calcDirLight(DirLight light, vec3 normal, vec3 frag2View) {
    vec3 ambient = light.ambient * vec3(texture(material0.diffuse, texCoord));

    float diff = max(dot(light.direction, normal), 0.0);
    vec3 diffuse = light.diffuse * vec3(texture(material0.diffuse, texCoord)) * diff;

    vec3 reflectDir = reflect(-light.direction, normal);
    frag2View = normalize(frag2View);
    float spec = pow(max(dot(reflectDir, frag2View), 0.0), material0.shininess);
    vec3 specular = light.specular * vec3(texture(material0.specular, texCoord)) * spec;

    return ambient + diffuse + specular;
};

// 点光源
struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define POINT_LIGHT_NUM 4
uniform PointLight pLight[POINT_LIGHT_NUM];

/**
*   计算点光源
*/
vec3 calcPointLight(PointLight light, vec3 normal, vec3 frag2View, vec3 fragPos) {
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    vec3 diffuseMap = vec3(texture(material0.diffuse, texCoord));

    vec3 ambient = light.ambient * diffuseMap;

    vec3 frag2Light = normalize(light.position - fragPos);
    float diff = max(dot(frag2Light, normal), 0.0);
    vec3 diffuse = light.diffuse * diffuseMap * diff;

    vec3 reflectDir = reflect(-frag2Light, normal);
    float spec = pow(max(dot(reflectDir, frag2View), 0.0), material0.shininess);
    vec3 specular = light.specular * vec3(texture(material0.specular, texCoord)) * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
};

// 聚光光源
struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutoff;
    float outerCutoff;
};
uniform SpotLight sLight;

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag2View, vec3 fragPos) {
    vec3 diffuseMap = vec3(texture(material0.diffuse, texCoord));

    vec3 ambient = light.ambient * diffuseMap;

    vec3 frag2Light = normalize(light.position - fragPos);
    float diff = max(dot(frag2Light, normal), 0.0);
    vec3 diffuse = light.diffuse * diffuseMap * diff;

    vec3 reflectDir = reflect(-frag2Light, normal);
    float spec = pow(max(dot(reflectDir, frag2View), 0.0), material0.shininess);
    vec3 specular = light.specular * vec3(texture(material0.specular, texCoord)) * spec;

    vec2 viewTexCoord = normalize(viewCoord).xy;
    vec3 spotLightMap = vec3(texture(material0.spotLightMap, viewTexCoord / 0.5 + 0.5)) * diff;

    // 巨坑！！之前做spotLight时，用的是自定义球型光源，light.direction就是原点指向光源位置的一个方向
    // 现在换成用相机做光源，但没注意light.direction的方向，是从相机指向出去的，导致theta计算始终有问题
    float theta = dot(normalize(frag2Light), normalize(-light.direction));
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    ambient *= intensity * attenuation;
    diffuse *= intensity * attenuation;
    specular *= intensity * attenuation;
    spotLightMap *= intensity * attenuation;

    // return ambient + diffuse + specular;
    // return ambient + spotLightMap + specular;
    return ambient + diffuse + spotLightMap + specular;
}

void main() {
    vec4 objColor = vec4(1.0);

    vec3 norm = normalize(vertexNormal);
    vec3 frag2View = normalize(viewPos - fragPos);

    vec3 result = vec3(0.0, 0.0, 0.0);
    // phase 1: directional lighting
    // result += calcDirLight(dLight, norm, frag2View);

    // phase 2: point lightings
    for(int i = 0; i < POINT_LIGHT_NUM; ++i) {
        // if(i == 1 || i == 2)
        result += calcPointLight(pLight[i], norm, frag2View, fragPos);
    }

    // phase 3: spot lighting
    result += calcSpotLight(sLight, norm, frag2View, fragPos);

    result = result * vec3(objColor);

    FragColor = vec4(result, 1.0);
}
