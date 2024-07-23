#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 vertexNormal;

struct Material {
    sampler2D ambient;
    sampler2D emission; // 发光贴图
    sampler2D diffuse;
    sampler2D specular;
    sampler2D spotLightMap; // 聚光灯贴图

    float shininess;
};
uniform Material material0;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLight dLight;

uniform bool blinn = true;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 frag2View) {
    vec3 ambient = light.ambient * vec3(texture(material0.diffuse, texCoords));

    float diff = max(dot(light.direction, normal), 0.0);
    vec3 diffuse = light.diffuse * vec3(texture(material0.diffuse, texCoords)) * diff;

    frag2View = normalize(frag2View);
    float spec = 0.0;
    if(blinn) {
        vec3 halfway = normalize(light.direction + frag2View);
        spec = pow(max(dot(halfway, normal), 0.0), material0.shininess);
    } else {
        vec3 reflectDir = reflect(-light.direction, normal);
        spec = pow(max(dot(reflectDir, frag2View), 0.0), material0.shininess);
    }
    vec3 specular = light.specular * vec3(texture(material0.specular, texCoords)) * spec;

    return ambient + diffuse + specular;
}

struct AttenuationFactor {
    float constant;
    float linear;
    float quadratic;
};
uniform AttenuationFactor aFactor;

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform PointLight pLight;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 frag2View, vec3 fragPos) {
    // calc the light attenuation factor
    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (aFactor.constant + aFactor.linear * dist + aFactor.quadratic * (dist * dist));

    vec3 diffuseMap = vec3(texture(material0.diffuse, texCoords));

    vec3 ambient = light.ambient * diffuseMap;

    normal = normalize(normal);
    vec3 frag2Light = normalize(light.position - fragPos);
    float diff = max(dot(frag2Light, normal), 0.0);
    vec3 diffuse = light.diffuse * diffuseMap * diff;

    frag2View = normalize(frag2View);
    float spec = 0.0;
    if(blinn) {
        vec3 halfway = normalize(frag2Light + frag2View);
        spec = pow(max(dot(halfway, normal), 0.0), material0.shininess);
    } else {
        vec3 reflectDir = reflect(-frag2Light, normal);
        spec = pow(max(dot(reflectDir, frag2View), 0.0), material0.shininess);
    }
    vec3 specular = light.specular * vec3(texture(material0.specular, texCoords)) * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

struct SpotLight {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float cutoff;
    float outerCutoff;
};
uniform SpotLight sLight;

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 frag2View, vec3 fragPos) {
    vec3 diffuseMap = texture(material0.diffuse, texCoords).rgb;

    vec3 ambient = light.ambient * diffuseMap;

    vec3 frag2Light = normalize(light.position - fragPos);
    float diff = max(dot(frag2Light, normal), 0.0);
    vec3 diffuse = light.diffuse * diffuseMap * diff;

    frag2View = normalize(frag2View);
    float spec = 0.0;
    if(blinn)
    {
        vec3 halfway = normalize(frag2Light + frag2View);
        spec = pow(max(dot(halfway, normal),0.0),material0.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-frag2Light, normal);
        spec = pow(max(dot(reflectDir, frag2View), 0.0),material0.shininess);
    }
    vec3 specular = light.specular * vec3(texture(material0.specular, texCoords)) * spec;

    float theta = dot(frag2Light, -light.direction);
    float epsilon = light.cutoff - light.outerCutoff;
    float intensity = clamp((theta - light.outerCutoff) / epsilon, 0.0, 1.0);

    float dist = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist * dist));

    ambient *= intensity * attenuation;    
    diffuse *= intensity * attenuation;    
    specular *= intensity * attenuation;    

    return ambient + diffuse + specular;
}

uniform sampler2D tex;

void main() {
    FragColor = texture(tex, texCoords);
}