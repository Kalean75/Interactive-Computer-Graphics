#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 txc;

uniform mat4 mvp;
uniform mat4 mv;

out vec2 texCoords;		


void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	//texCoords = txc;
	texCoords = vec2(txc + vec2(1,1))/2.0;
}