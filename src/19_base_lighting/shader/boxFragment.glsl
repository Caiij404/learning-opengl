#version 330 core
out vec4 FragColor;

in vec3 vertexNormal;
in vec3 fragPos;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objColor;
uniform vec3 viewPos;

void main() {
    // 环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射
    vec3 frag2light = normalize(lightPos - fragPos);
    vec3 norm = normalize(vertexNormal);
    float diff = max(dot(frag2light, norm), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面反射
    float specularStrength = 0.5;
    vec3 frag2view = normalize(viewPos - fragPos);
    vec3 reflectDir = reflect(-frag2light, norm);
    float spec = pow(max(dot(frag2view, reflectDir), 0.0), 256);
    vec3 specular = specularStrength * spec * lightColor;


    vec3 result = (ambient + diffuse + specular) * objColor;

    FragColor = vec4(result, 1.0);
}