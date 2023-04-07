#version 410 core

layout(quads, equal_spacing, ccw) in;

in vec2 outtexCoords[];
in vec3 outviewPos[];
in vec3 outlightDirection[];
in mat3 outmatrixNormal[];
in vec3 outnormCoords[];
in float inTessLev[];
in mat4 mvpin[];
uniform sampler2D dispMap;

out vec2 texCoords;
out vec3 normCoords;
out vec3 viewPos;
out vec3 lightDirection;
out mat3 matrixNormal;
out mat4 mpv;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3)
{
	vec4 a = mix(v0, v1, gl_TessCoord.x);
	vec4 b = mix(v3, v2, gl_TessCoord.x);

	return mix(a,b,gl_TessCoord.y);
}
void main()
{
	
	float u = gl_TessCoord.x;
	float v = gl_TessCoord.y;

	vec4 pos0 = gl_in[0].gl_Position;
	vec4 pos1 = gl_in[1].gl_Position;
	vec4 pos2 = gl_in[2].gl_Position;
	vec4 pos3 = gl_in[3].gl_Position;
	vec4 displacement = vec4(outnormCoords[0] * (texture2D(dispMap,outtexCoords[0]).r), 0.0);

	vec4 leftPos = pos1 + v * (pos3-pos1);
	vec4 rightPos = pos0 + v * (pos2 - pos0);
	vec4 pos = leftPos + u * (rightPos - leftPos);
	gl_Position = pos;

	vec2 txc0 = outtexCoords[0];
	vec2 txc1 = outtexCoords[1];
	vec2 txc2 = outtexCoords[2];
	vec2 txc3 = outtexCoords[3];

	vec2 leftTxc = txc1 + v * (txc3-txc1);
	vec2 rightTxc = txc0 + v * (txc2 - txc0);
	vec2 txc = leftTxc + u * (rightTxc - leftTxc);

	//viewPos
	vec3 vp0 = outviewPos[0];
	vec3 vp1 = outviewPos[1];
	vec3 vp2 = outviewPos[2];
	vec3 vp3 = outviewPos[3];

	vec3 leftvp = vp1 + v * (vp3-vp1);
	vec3 rightvp = vp0 + v * (vp2 - vp0);
	vec3 vp = leftvp + u * (rightvp - leftvp);

	//normCoords
	vec3 nc0 = outnormCoords[0];
	vec3 nc1 = outnormCoords[1];
	vec3 nc2 = outnormCoords[2];
	vec3 nc3 = outnormCoords[3];

	vec3 leftnc = nc1 + v * (nc3-nc1);
	vec3 rightnc = nc0 + v * (nc2 - nc0);
	vec3 nc = leftnc + u * (rightnc - leftnc);

	//light
	vec3 ld0 = outlightDirection[0];
	vec3 ld1 = outlightDirection[1];
	vec3 ld2 = outlightDirection[2];
	vec3 ld3 = outlightDirection[3];

	vec3 leftld = ld1 + v * (ld3-ld1);
	vec3 rightld = nc0 + v * (ld2 - ld0);
	vec3 ld = leftld + u * (rightld - leftld);

	matrixNormal = outmatrixNormal[0];
	texCoords = txc;
	viewPos = vp;
	normCoords = nc;
	lightDirection = ld;
}