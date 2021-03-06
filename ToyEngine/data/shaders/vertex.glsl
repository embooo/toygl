#version 430 core

layout(location= 0) in vec3 position;
layout(location= 1) in vec3 normal;
layout(location= 2) in vec4 tangeant;
layout(location= 3) in vec2 texcoord0;
layout(location= 4) in vec2 texcoord1;
layout(location= 5) in int  materialID;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 texcoord[2];

out mat4 modelMat;
out mat4 viewMat;

out vec3 p; // position du point de la surface dans le rep?re monde
out vec3 n; // normale du point de la surface dans le rep?re monde
out vec4 t; // rep?re tangeant 
out vec3 b;
out int matId;

void main( )
{
    mat4 mvp = projection * view * model;
    vec4 pos = mvp * vec4(position, 1.0);

    modelMat = model;
    viewMat  = view;
    
    texcoord[0] = texcoord0;
    texcoord[1] = texcoord1;

    p     = vec3(model * vec4(position, 1.0));
    n     = vec3(model * vec4(normal,   0.0));
    t     = tangeant;
    matId = materialID;
    
    gl_Position = mvp * vec4(position, 1.0);
}