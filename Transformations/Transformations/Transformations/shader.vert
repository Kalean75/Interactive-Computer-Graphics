#version 330 core

layout (location = 0 ) in vec3 pos;

uniform mat4 ModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;


void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
}