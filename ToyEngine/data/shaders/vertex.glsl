#version 430 core

layout(location= 0) in vec3 position;
layout(location= 1) in vec3 normal;
layout(location= 2) in vec2 texcoord0;
layout(location= 3) in vec2 texcoord1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vNormal; // vertex normal 
out mat4 viewMat;
out vec2 vTexcoord;

out vec3 p; // position du point de la surface dans le repère monde

void main( )
{
    mat4 mvp = projection * view * model;
    viewMat = view;
    vNormal = normal;
    
    gl_Position =  mvp * vec4(position, 1.0);
}