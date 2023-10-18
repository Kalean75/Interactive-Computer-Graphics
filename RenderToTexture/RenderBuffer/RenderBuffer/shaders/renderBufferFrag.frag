#version 330 core

in vec2 texCoords;
uniform sampler2D text;

layout(location = 0 ) out vec4 color;

void main()
{
	color = texture2D(text,texCoords) + vec4(0.2,0.2,0.2,1.0);
	//color = vec4(1.0,1.0,1.0,1.0);
} 