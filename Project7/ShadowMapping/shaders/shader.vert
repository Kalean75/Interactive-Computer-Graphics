#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norms;
layout (location = 2) in vec2 txc;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 normalMatrix;
uniform mat4 matrixShadow;

uniform mat4 light;
uniform vec3 lightPos;

out vec3 viewPos;
out vec3 normalPos;
out vec3 lightDirection;
out vec2 texCoords;
out vec4 lightView_Position;

void main()
{
	texCoords=txc;
	lightDirection = vec3(light);
	vec4 vertPos4 = mv * vec4(pos, 1.0);
	viewPos = vec3(vertPos4)/ vertPos4.w;
	normalPos = vec3(normalMatrix * norms);
	gl_Position = mvp * vec4(pos, 1.0);
	lightView_Position = matrixShadow * vec4(pos,1);
}