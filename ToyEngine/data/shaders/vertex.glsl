#version 430 core

layout(location= 0) in vec3 position;
layout(location= 1) in vec3 normal;
layout(location= 2) in vec2 texcoord0;
layout(location= 3) in vec2 texcoord1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vertexPos;
out vec3 vertexNormal; 
out mat4 viewMat;
out vec2 vTexcoord0;
out vec2 vTexcoord1;

out vec2 texcoords[2];

out mat4 matModel;

out vec3 p; // position du point de la surface dans le rep�re monde

void main( )
{
    mat4 mvp = projection * view * model;
    viewMat = view;

    vec4 pos =  mvp * vec4(position, 1.0);
    
    texcoords[0] = texcoord0;
    texcoords[1] = texcoord1;
    vertexPos    = position;
    vertexNormal = normal;
    matModel = model;
    
    gl_Position = mvp * vec4(position, 1.0);
}