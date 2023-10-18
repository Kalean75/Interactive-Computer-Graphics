#version 410 core
layout(vertices = 4) out;

in vec2 texCoords[];
in vec3 normCoords[];
in vec3 viewPos[];
in vec3 lightDirection[];
in mat3 matrixNormal[];
in mat4 mvpout[];

out vec2 outtexCoords[];
out vec3 outviewPos[];
out vec3 outlightDirection[];
out mat3 outmatrixNormal[];
out vec3 outnormCoords[];
out float inTessLev[];
out mat4 mvpin[];

in float tessLev[];

void main()
{
	/**gl_TessLevelOuter[0] = 1.0f;
	gl_TessLevelOuter[1] = 1.0f;
	gl_TessLevelOuter[2] = 1.0f;
	gl_TessLevelOuter[3] = 1.0f;

	gl_TessLevelInner[0] = 1.0f;
	gl_TessLevelInner[1] = 1.0f;**/

	gl_TessLevelOuter[0] = 1.0f + tessLev[0];
	gl_TessLevelOuter[1] = 1.0f + tessLev[1];
	gl_TessLevelOuter[2] = 1.0f + tessLev[2];
	gl_TessLevelOuter[3] = 1.0f + tessLev[3];

	gl_TessLevelInner[0] = 1.0f + tessLev[0];
	gl_TessLevelInner[1] = 1.0f + tessLev[1];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position + tessLev[gl_InvocationID];

	outtexCoords[gl_InvocationID] = texCoords[gl_InvocationID]+ tessLev[gl_InvocationID];
	outviewPos[gl_InvocationID] = viewPos[gl_InvocationID]+ tessLev[gl_InvocationID];
	outlightDirection[gl_InvocationID] = lightDirection[gl_InvocationID]+ tessLev[gl_InvocationID];
	outmatrixNormal[gl_InvocationID] = matrixNormal[gl_InvocationID]+ tessLev[gl_InvocationID];
	outnormCoords[gl_InvocationID] = normCoords[gl_InvocationID]+ tessLev[gl_InvocationID];
	inTessLev[gl_InvocationID] = tessLev[gl_InvocationID];
	mvpin[gl_InvocationID] = mvpout[gl_InvocationID];
	
}