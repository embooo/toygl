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
uniform vec4      baseColorFactor;
uniform int       currentTexcoord;

// Varyings
in  vec3 vPosition; 
in  vec3 vNormal; 
in  vec2 vTexcoord0; 
in  vec2 vTexcoord1; 
out vec4 fragment_color;
in vec2  texcoords[2];
in vec3  cameraPos;
in mat4 viewMat;

// Constants
const float PI = 3.14;

#define BLINN

void main( )
{
    vec3 normal   = normalize(vNormal);
    vec3 albedo   = vec3(0.2, 0.2, 0.2);

    DirectionalLight light;
    light.dir = normalize( vec3(0.6, -0.35, -0.1) );
    light.ambient = vec3(14.0, 9.0, 5.0);

    Sky sky;
    sky.ambient = vec3(1.0, 1.6, 1.8);
    sky.diffuse = sky.ambient * dot(0.5 + 0.5 * vec3(0, 1, 0), normal);

    vec3 color =  albedo ;

    float cosTheta = max(0, dot(normal, -light.dir));

    light.diffuse  = cosTheta * light.ambient;

    #ifdef BLINN
        vec3 O = normalize(cameraPos - vPosition); // observateur 
        vec3 L = normalize(-light.dir); // lumière 

        // bissectrice
        vec3 H = normalize(O + L);
    
        float alpha = 2.0;
        float cosTheta_H = dot(normal, H);

        // brdf
        float fr = ( (alpha + 8) / (8 * PI)  ) * pow(cosTheta_H, alpha);

        color *= fr * light.diffuse;
    #endif
        
    color += albedo * sky.diffuse;
    
    vec4 baseColorTex = texture(baseColorTexture, texcoords[currentTexcoord]);


    fragment_color  = vec4(color, 1.0) * baseColorTex;   
}

