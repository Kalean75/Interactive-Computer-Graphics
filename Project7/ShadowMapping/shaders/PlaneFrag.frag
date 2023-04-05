#version 330 core
layout(location = 0 ) out vec4 color;
in vec4 lightView_Position;

uniform sampler2D shadow;

void main()
{
	color = vec4(0.5,0.5,0.5,0.5);
	vec3 p = lightView_Position.xyz/lightView_Position.w;

	color *= texture(shadow,p.xy).r < p.z ? 0 : 1;
} 