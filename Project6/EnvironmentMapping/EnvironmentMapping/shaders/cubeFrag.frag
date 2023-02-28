#version 330 core


layout(location = 0) out vec4 color;
in vec3 texCoords;
uniform samplerCub env;

void main()
{
	color = vec4(1.0f,1.0f,1.0f,1.0f);
	//color = texture(env,texCoords);
} 