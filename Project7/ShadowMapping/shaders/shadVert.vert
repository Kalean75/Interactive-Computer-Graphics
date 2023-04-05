#version 330 core
layout (location = 0) in vec3 pos;


uniform mat4 mvp;
uniform mat4 matrixShadow;

out vec4 lightView_Position;

void main()
{

	gl_Position = mvp * vec4(pos, 1.0);
	lightView_Position = matrixShadow * vec4(pos,1);
}