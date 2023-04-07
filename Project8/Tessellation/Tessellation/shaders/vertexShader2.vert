#version 330 core
layout (location = 0) in vec3 pos;

uniform mat4 mvp;
uniform mat4 mv;
uniform float tessellationLevel;
out float tessLev;

void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	tessLev = tessellationLevel;
}