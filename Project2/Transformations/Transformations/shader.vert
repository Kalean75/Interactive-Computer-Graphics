#version 330 core

in vec3 position;

uniform mat4 ModelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;


void main()
{
	gl_Position = projectionMatrix * viewMatrix * ModelMatrix * vec4(position, 1.0);
}