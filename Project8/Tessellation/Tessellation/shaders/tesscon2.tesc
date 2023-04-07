#version 410 core
layout(vertices = 4) out;

in float tessLev[];

void main()
{
	gl_TessLevelOuter[0] = 1.0f + tessLev[0];
	gl_TessLevelOuter[1] = 1.0f+ tessLev[1];
	gl_TessLevelOuter[2] = 1.0f+ tessLev[2];
	gl_TessLevelOuter[3] = 1.0f+ tessLev[3];

	gl_TessLevelInner[0] = 1.0f+ tessLev[0];
	gl_TessLevelInner[1] = 1.0f+ tessLev[1];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}