#version 330 core
out vec4 FragColor;
in vec2 texCoords;
in vec3 vertexNormal;
in vec3 fragPos;

uniform bool blinn = true;

uniform sampler2D woodMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    vec3 color = texture(woodMap, texCoords).rgb;

    vec3 ambient = 0.05 * color;

    vec3 normal = normalize(vertexNormal);
    vec3 frag2light = normalize(lightPos - fragPos);

    float diff = max(dot(frag2light, normal), 0.0);
    vec3 diffuse = diff * color;

    vec3 frag2view = normalize(viewPos - fragPos);
    float spec = 0.0;
    if(blinn) {
        vec3 halfway = normalize(frag2light + frag2view);
        spec = pow(max(dot(halfway, normal), 0.0), 16.0);
    } else {
        vec3 reflectDir = reflect(-frag2light, normal);
        spec = pow(max(dot(normal, reflectDir), 0.0), 8.0);
    }
    vec3 specular = spec * vec3(0.3);

    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, 1.0);
}