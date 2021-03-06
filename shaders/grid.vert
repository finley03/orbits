// Screen vertex shader

#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = (model * vec4(aPos, 1.0)).xyz;
//	gl_Position = projection * view * vec4(aPos, 1.0);
//	FragPos = aPos;
}
