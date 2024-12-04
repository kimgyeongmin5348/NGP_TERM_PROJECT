#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec3 vNormal;

out vec3 FragPos;
out vec3 Normal;

uniform vec3 inColor;
out vec3 outColor;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

void main(void) 
{

	gl_Position = projection * view * transform * vec4(vPos, 1.0);
	FragPos = vec3(transform * vec4(vPos,1.0));
	Normal = vec3(transform * vec4(vNormal,1.0));
	outColor = inColor;
}