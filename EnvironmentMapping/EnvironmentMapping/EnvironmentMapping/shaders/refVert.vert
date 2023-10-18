#version 330 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norms;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normalMatrix;

uniform mat4 light;
uniform vec3 lightPos;

out vec3 viewPos;
out vec3 normalPos;
out vec3 lightDirection;	


void main()
{
	lightDirection = vec3(light * vec4(lightPos,1.0));
	vec4 vertPos4 = mv * vec4(pos, 1.0);
	viewPos = vec3(vertPos4)/ vertPos4.w;
	normalPos = vec3(normalMatrix * norms);
	gl_Position = mvp * vec4(pos, 1.0);
}