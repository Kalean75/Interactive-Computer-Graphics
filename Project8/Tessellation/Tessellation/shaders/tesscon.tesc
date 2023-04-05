#version 410 core
layout(vertices = 4) out;

in vec3 myInData[];

out vec3 myData[];

void main()
{
	gl_TessLevelOuter[0] = 2.0;
	gl_TessLevelOuter[1] = 4.0;
	gl_TessLevelOuter[2] = 6.0;
	gl_TessLevelOuter[3] = 8.0;

	gl_TessLevelOuter[0] = 8.0;
	gl_TessLevelOuter[1] = 8.0;

	gl_out[gl_InvocationID].gl_position = gl_in[gl_InvocationID].gl_Position;
	myData[gl_InvocationID] = myInData[gl_InvocationID];
}