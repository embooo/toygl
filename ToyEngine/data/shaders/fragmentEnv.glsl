#version 430 core

struct DirectionalLight
{
    vec3 dir;

    vec3 ambient;
    vec3 diffuse;
};

struct Sky
{
    vec3 ambient;
    vec3 diffuse;
};

// Uniforms 
uniform sampler2D baseColorTexture;

// Varyings
in  vec3 vNormal; 
in  vec2 vTexcoord0; 
in  vec2 vTexcoord1; 
out vec4 fragment_color;

// Constants
const float PI = 3.14;

void main( )
{

    vec3 normal   = normalize(vNormal);
    vec3 albedo   = vec3(0.2, 0.2, 0.2);

    DirectionalLight light;
    light.dir = normalize(vec3(0.2, -0.15, -0.1));
    light.ambient = vec3(7.0, 4.5, 2.5);

    Sky sky;
    sky.ambient = vec3(0.5, 0.8, 0.9);
    sky.diffuse = sky.ambient * dot(0.5 + 0.5 * vec3(0, 1, 0), normal);

    float cosTheta = dot(normal, -light.dir);

    light.diffuse  = cosTheta * light.ambient;

    vec3 color =  albedo * light.diffuse;
         color += albedo * sky.diffuse;
    
    vec4 currentTexture = texture(baseColorTexture, vTexcoord0);

    fragment_color  = vec4(color, 1.0) * currentTexture;   
}

