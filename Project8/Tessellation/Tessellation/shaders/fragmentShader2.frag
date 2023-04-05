#version 330 core
layout(location = 0) out vec4 color;

uniform sampler2D normalMap;

void main()
{
	color = vec4(1.0,1.0,0.0,1.0);
} 