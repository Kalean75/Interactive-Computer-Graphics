#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 txc;
layout (location = 2) in vec3 norms;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat4 light;
uniform mat3 normalMatrix;
uniform vec3 lightPos;
uniform bool hasDisplacement;

out vec2 texCoords;
out vec3 normCoords;
out vec3 viewPos;
out vec3 lightDirection;
out mat3 matrixNormal;
out mat4 mvpout;

void main()
{
	texCoords = txc;
	lightDirection = vec3(light)*lightPos;
	normCoords = vec3(normalMatrix * norms);
	//viewPos = normalize(-vec3(mv*vec4(pos,1)));
	vec4 vertPos4 = mv * vec4(pos, 1.0);
	viewPos = vec3(vertPos4)/ vertPos4.w;
	matrixNormal = normalMatrix;
	mvpout = mvp;
	gl_Position = mvp * vec4(pos, 1.0);

}