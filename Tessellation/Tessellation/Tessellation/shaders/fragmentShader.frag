#version 330 core
layout(location = 0) out vec4 color;

uniform sampler2D normalMap;
//uniform sampler2D dispMap;

in vec2 texCoords;
in vec3 normCoords;
in vec3 viewPos;
in vec3 lightDirection;
in mat3 matrixNormal;

void main()
{

	float shininess = 5.0;
	vec3 v = -normalize(viewPos);
	//vec3 n = normalize(normCoords);
	vec3 n = normalize(matrixNormal* ((texture(normalMap, texCoords).xyz * 2.0f - 1.0f)));
	vec3 l = normalize(lightDirection);
	vec3 h = normalize(l + v);


	vec4 I = vec4(1.0,1.0,1.0,1.0);
	float theta = max(dot(l,n), 0.0);
	float phi = pow(max(dot(n,h),0.0), shininess);

	vec4 Kd = vec4(0.5,0.5,0.5,1.0);
	vec4 Ks = vec4(0.5,0.5,0.5,1.0);


	//vec4 ambient = vec4(vec3(0.5,0.5,0.5) * Ks.rgb,1.0);
	vec4 specular = vec4(Ks.rgb* phi,1.0);
	color = I * (Kd*theta+specular);
} 