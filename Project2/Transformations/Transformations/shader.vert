#version 330 core
layout (location = 0) in vec3 position;
uniform mat4 mvp;

out vec4 mainColor;

void main()
{
    gl_Position =  mvp * vec4( 0.05 *position.x,  0.05 *position.y, 0.05 *position.z, 1.0); 
    mainColor = vec4(1.0f,0.0f,0.0f,1.0f);
}