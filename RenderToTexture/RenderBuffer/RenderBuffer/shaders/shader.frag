#version 330 core

in vec3 norms;
in vec3 normalPos;
in vec3 viewPos;
in vec3 lightDirection;
in vec2 texCoords;

uniform sampler2D texKd;
uniform sampler2D texKs;

out vec4 color;

void main()
{
	//vec4 Ks = vec4(1.0,1.0,1.0,1.0);
	//vec4 Ks = vec4(1.0,1.0,1.0,1.0);
	float shininess = 2.0;
	vec3 v = -normalize(viewPos);
	vec3 n = normalize(normalPos);
	vec3 l = normalize(lightDirection);
	vec3 h = normalize(l + v);

	vec4 Ks = vec4(texture2D(texKs, texCoords));
	vec4 I = vec4(1.0,1.0,1.0,1.0);
	float theta = max(dot(l,n), 0.0);
	float phi = pow(max(dot(n,h),0.0), shininess);

	vec4 Kd = vec4(texture2D(texKd, texCoords));

	vec4 ambient = vec4(vec3(0.25,0.25,0.25) * Ks.rgb,1.0);
	vec4 specular = vec4(Ks.rgb* phi,1.0);
	
	color = I * (Kd*theta+specular) + ambient;
	//color = vec4(normalPos, 1.0f);
} 