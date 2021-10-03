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


struct PointLight
{
    vec3 pos;

    vec3 ambient;
    vec3 diffuse;
};

// Uniforms 

// Metallic-Rougness Workflow
uniform sampler2D baseColorTexture;
uniform vec4      baseColorFactor;
uniform int       currentBaseColorTexcoord;

uniform sampler2D normalTexture;
uniform int currentNormalTexcoord;


// Varyings
in  vec3 vertexPos; 
in  vec3 vertexNormal; 
in  vec2 vTexcoord0; 
in  vec2 vTexcoord1; 
in  mat4 matModel; 
out vec4 fragment_color;
in vec2  texcoords[2];
in vec3  cameraPos;
in mat4 viewMat;

// Constants
const float PI = 3.141592;
const vec3  MATE = vec3(0.2, 0.2, 0.2);

#define BLINN
//#define PER_FRAGMENT_NORMALS

void main( )
{

    vec3 normalWS     = normalize( vec3( matModel * vec4(vertexNormal, 0.0) ) );
    vec3 positionWS   = normalize( vec3( matModel * vec4(vertexPos, 0.0) ) );

    #ifdef PER_FRAGMENT_NORMALS
        vec4 normalTex    = texture(normalTexture, texcoords[0]) ;  
        normalWS          = vec3( normalize (normalTex * 2.0  - 1.0 ) * matModel ); // value is in [-1,1] range
    #endif

    vec3 color =  MATE ;

    DirectionalLight dirLight;
    dirLight.dir     = normalize( vec3(-0.0f, -0.1f, -0.1f) );
    dirLight.ambient = vec3(7.0, 4.5, 2.5) * 2.0;

    PointLight pointLight;
    pointLight.pos = vec3(matModel * vec4(0,100,0,1));

    pointLight.ambient = vec3(7.0, 4.5, 2.5);

    Sky sky;
    sky.ambient = vec3(1.4, 0.9, 0.5);
    sky.diffuse = sky.ambient * dot(normalize(vec3(0.0, -0.6, 0.0)), normalWS) ;

    float cosTheta = max(0, dot(normalWS, normalize(pointLight.pos - positionWS)));

    pointLight.diffuse  = cosTheta * pointLight.ambient;
    dirLight.diffuse    = cosTheta * dirLight.ambient ;

    #ifdef BLINN

        vec3 O = normalize(cameraPos - positionWS); // observateur 
        vec3 L = normalize(pointLight.pos - positionWS); // lumière 

        // bissectrice
        vec3 H = normalize(O + L);
    
        float alpha = 0.1;
        float cosTheta_H = dot(normalWS, H);

        // brdf
        float fr =  ( (alpha + 8) / (8 * PI)  ) * pow(cosTheta_H, alpha);

        color *= fr * pointLight.diffuse;
    #endif

    color += MATE * sky.diffuse;

    vec4 baseColorTex = texture(baseColorTexture, texcoords[currentBaseColorTexcoord]);

    // trans
    if(baseColorTex.a < 0.3) discard;

    fragment_color  = vec4(color, 1.0) * baseColorTex ;   

}

