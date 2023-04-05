#version 400 core

layout (triangles, invocations = 1) in;
layout(line_strip,max_vertices=4) out;
//layout(triangle_strip,max_vertices=4) out;

void main()
{
	gl_Position = gl_in[gl_InvocationID].gl_Position + vec4(0.0,0.0,0.0,0.01);
	EmitVertex();

	gl_Position = gl_in[gl_InvocationID + 1].gl_Position + vec4(0.0,0.0,0.0,0.01);
	EmitVertex();

	gl_Position = gl_in[gl_InvocationID + 2].gl_Position + vec4(0.0,0.0,0.0,0.01);
	EmitVertex();

	gl_Position = gl_in[gl_InvocationID].gl_Position + vec4(0.0,0.0,0.0,0.01);
	EmitVertex();

	EndPrimitive();
}