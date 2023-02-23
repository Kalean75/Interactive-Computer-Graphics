#version 330 core

in vec2 texCoords;
uniform sampler2D text;

layout(location = 0 ) out vec4 color;

void main()
{
	color = vec4(texture2D(text,texCoords).rgb,1.0f);
	//color = vec4(1.0,1.0,1.0,1.0);
} 