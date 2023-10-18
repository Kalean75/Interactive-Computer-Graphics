#version 410 core

layout(quads, equal_spacing, ccw) in;

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

	vec4 leftPos = pos1 + v * (pos3-pos1);
	vec4 rightPos = pos0 + v * (pos2 - pos0);
	vec4 pos = leftPos + u * (rightPos - leftPos);
	gl_Position = pos;

	//gl_Position = interpolate(gl_in[0].gl_Position, gl_in[1].gl_Position,gl_in[2].gl_Position,gl_in[3].gl_Position);


}