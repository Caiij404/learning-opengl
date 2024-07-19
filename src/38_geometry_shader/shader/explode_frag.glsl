#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D modelTex;
void main(){
    FragColor = texture(modelTex, TexCoords);
}

