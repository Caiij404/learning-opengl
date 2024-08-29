#version 330 core
layout(location = 0) in vec3 Position;

out vec3 outTexCoord;

uniform mat4 model = mat4(1.0);
uniform mat4 view;
uniform mat4 projection;

void main() {
	outTexCoord = normalize(Position);
	vec4 pos = projection * view * model * vec4(Position, 1.0);
	// vec4 pos = projection * view * vec4(Position, 1.0);
	gl_Position = pos.xyww;
}