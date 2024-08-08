#version 330 core
out vec4 FragColor;

uniform sampler2D diffuseMap; // 贴图
uniform sampler2D normalMap; // 贴图
uniform sampler2D depthMap;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform float strength;
uniform float height_scale = 1.0;
uniform float flag = 0.0;
// vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {
//     float height = texture(depthMap, texCoords).r;
//     vec2 p;
//     if(flag < 5.0) {
//         p = viewDir.xy / viewDir.z * (height * height_scale);
//     } else {
//         p = (viewDir * height * height_scale).xy;
//     }
//     return texCoords - p;
// }

vec2 SteepParallaxMapping(vec2 texCoords, vec3 viewDir) {
    const float minLayers = 8;
    const float maxLayers = 32;

    // number of depth layers
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer 
    float currentLayerDepth = 0.0;
    // the amount to shift the texture corrdinates per layer from vector P
    vec2 p = viewDir.xy * height_scale;
    vec2 deltaTexCoords = p / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    return currentTexCoords;
}

vec2 ParallaxOcclusionMapping(vec2 texCoords, vec3 viewDir) {
    const float minLayers = 8;
    const float maxLayers = 32;

    // number of depth layers
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));

    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer 
    float currentLayerDepth = 0.0;
    // the amount to shift the texture corrdinates per layer from vector P
    vec2 p = viewDir.xy * height_scale;
    vec2 deltaTexCoords = p / numLayers;

    vec2 currentTexCoords = texCoords;
    float currentDepthMapValue = texture(depthMap, currentTexCoords).r;
    while(currentLayerDepth < currentDepthMapValue) {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = texture(depthMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 preTexCoords = currentTexCoords + deltaTexCoords;

    // 碰撞后的深度差 H(T3)   currentLayerDepth > currentDepthMapValue  
    float afterDepth = currentLayerDepth - currentDepthMapValue;
    // 碰撞前的深度差 H(T2)   texture(depthMap, preTexCoords).r > currentLayerDepth - layerDepth (上一层深度)
    float beforeDepth = texture(depthMap, preTexCoords).r - (currentLayerDepth - layerDepth);

    // 当afterDepth趋于0，则weight趋于0；当beforeDepth趋于0，则weight趋于1
    float weight = afterDepth / (afterDepth + beforeDepth);
    // interpolation 插值 of texture coordinates 
    vec2 finalTexCoords = preTexCoords * weight + currentTexCoords * (1.0 - weight);
    return finalTexCoords;
}
void main() {

    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec2 texCoords;
    if(flag < 5.0) {
        texCoords = SteepParallaxMapping(fs_in.TexCoords, viewDir);
    } else {
        texCoords = ParallaxOcclusionMapping(fs_in.TexCoords, viewDir);
    }

    // 丢弃失真片段
    // 纹理坐标超出范围
    if(texCoords.x > 1.0 || texCoords.x < 0.0 || texCoords.y > 1.0 || texCoords.y < 0.0) {
        discard;
    }
    float gamma = 2.2;
    vec3 color = pow(texture(diffuseMap, texCoords).rgb, vec3(gamma));

    vec3 ambient = strength * color; // 环境光

    //vec3 normal = normalize(oNormal);
    // 从法线贴图获取[0,1]范围的法线值
    vec3 normal = texture(normalMap, texCoords).rgb;
    // 将法线向量转换到[-1，1]范围
    normal = normalize(normal * 2.0 - 1.0);

    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color; // 漫反射

    // vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);

    vec3 specular = vec3(0.3) * spec; // 镜面光
    vec3 result = (ambient + diffuse + specular);

    FragColor = vec4(result, 1.0);

    FragColor.rgb = pow(FragColor.rgb, vec3(1.0 / gamma));

}