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
    vec3 color;
};

// Uniforms 

// Metallic-Rougness Workflow
uniform sampler2D baseColorTexture;
uniform vec4      baseColorFactor;
uniform int       currentBaseColorTexcoord;


uniform float     roughnessFactor;
uniform sampler2D metallicRoughnessTexture;
uniform int       currentMetallicRoughnessTexcoord;

uniform sampler2D normalTexture;
uniform int currentNormalTexcoord;

uniform sampler2D emissiveTexture;
uniform vec3 emissiveFactor;


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

//#define BLINN
//#define LAMBERT
//#define COOK
#define COOK_GKIT


void main( )
{
    // Positions
    vec3 p = vertexPos;
    vec3 source = normalize(vec3(-0.1, -0.1, -0.08));

    // directions
    vec3 n = normalize(vertexNormal);  // surface normal
    vec3 o = normalize(cameraPos - p); // view (camera) direction 
   // vec3 l = normalize(source - p);   // light direction 
    vec3 l = normalize(-source);   // light direction (directional light)

    vec3 h = normalize(o + l);       // bissectice
    
    float cosTheta   = max(0, dot(n, l));
    float cosTheta_h = max(0, dot(n, h));
    float cosTheta_o = max(0, dot(n, o));

//    if(cosTheta <= 0 || cosTheta_h <= 0 || cosTheta_o <= 0)
//    return;

    // Colors
    vec3 diffuse = vec3(1.0, 1.0, 1.0);
    vec3 color = diffuse;

//////////////////////////////////////////////////////////////////////////////////////// LAMBERT
    #ifdef LAMBERT
        float k = 1.0; // diffuse albedo, constant  [0, 1]
        float fr = k / PI ;
        color = fr * diffuse * cosTheta;
    #endif

    #ifdef PER_FRAGMENT_NORMALS
        vec4 tangeantSpaceNormal    = texture(normalTexture, texcoords[currentNormalTexcoord]) ;  
        vec3 t                      = vec3( matModel * vec4(tangeantSpaceNormal.xyz, 0.0) ); // tangeant
        vec3 b                      = vec3( matModel * vec4( cross(vertexNormal, tangeantSpaceNormal.xyz) * tangeantSpaceNormal.w, 0.0) ); // bitangeant
        mat3 tbn                    = mat3(t, b, n);
        
        n                           = tbn * normalize(tangeantSpaceNormal.xyz * 2.0 - 1.0);
    #endif
//////////////////////////////////////////////////////////////////////////////////////// BLINN
    #ifdef BLINN
        float alpha = 2.0;
        // brdf
        float fr =   (alpha + 8) / (8.0 * PI)  * pow(cosTheta_h, alpha);

        color = fr * diffuse * cosTheta;
    #endif
//////////////////////////////////////////////////////////////////////////////////////// Cook-Torrance : Microfacets
    vec4 baseColorTex  = texture(baseColorTexture, texcoords[currentBaseColorTexcoord]);
    vec4 metallicRough = texture(metallicRoughnessTexture, texcoords[currentMetallicRoughnessTexcoord]);

    vec4 emissive = texture(emissiveTexture, texcoords[0]);


    // Multiple light sources
    PointLight light[4];
    light[0].pos = vec3(0,0.1,0);   
    light[0].color = vec3(1, 0, 0);   

    light[1].pos = vec3(0,0.1,0.5);
    light[1].color = vec3(0, 1, 0);

    light[2].pos = vec3(0,0.1,1);
    light[2].color = vec3(0, 0, 1);

    light[3].pos = vec3(0,0.1,1.5);
    light[3].color = vec3(1, 1, 0);

    vec3 totalRadiance = vec3(0.0);
    for(int i=0; i < 4; i++)
    {
        vec3 l = normalize(light[i].pos - p); // incident light
        vec3 h = normalize(o + l);

        float dist = length(light[i].pos - p);
        float attenuation = 1 / (dist*dist);
        vec3 radiance = light[i].color * attenuation;

        totalRadiance += radiance;
    }

    #ifdef COOK
        // Normal distribution function D(h)
        float Roughness = metallicRough.y * roughnessFactor;// * roughnessFactor;

        float alpha = Roughness * Roughness;
        float aa = alpha * alpha;
        float d = ((cosTheta_o * cosTheta_o) * (aa - 1) + 1);

        float D = aa / (PI * (d*d)) ;

        // Geometric attenuation (microfacets masking/shadowing)
        // Schlick
        float k = alpha/2;
        float G1_v = cosTheta_o / ( (cosTheta_o) * (1 - k) + k );
        float G1_l = cosTheta / ( (cosTheta) * (1 - k) + k);
        float G = G1_l * G1_v;

        // Fresnel reflectance
        float r = 2.5;
        float F0 = pow((r - 1), 2) / pow((r + 1), 2);
        float dotVH = max(0, dot(o, h));
        float F = F0 + (1.0 - F0) * pow(2.0, (-5.55473 * dotVH - 6.98316) * dotVH);
        float fr = (D * F * G) / ( 4.0 * cosTheta * cosTheta_o) ;

        color = fr * diffuse * cosTheta;//* diffuse * cosTheta;
    #endif

    
    #ifdef COOK_GKIT
    // D
    float Roughness = metallicRough.y * roughnessFactor;
    float alpha = Roughness * Roughness;
    float D= alpha / (1 + cosTheta_h*cosTheta_h * (alpha*alpha -1));
    D= D*D / PI;

    // G2
    float tan2_theta_o= 1 / (cosTheta_o*cosTheta_o) - 1;
    float lambda_o= 1 + alpha*alpha * tan2_theta_o;
    float tan2_theta= 1 / (cosTheta*cosTheta) - 1;
    float lambda= 1 + alpha*alpha * tan2_theta;
    float G2= 2 / (sqrt(lambda_o) + sqrt(lambda));

    // F
    float r = 2.5;
    float F0 = pow((r - 1), 2) / pow((r + 1), 2);
    float F= F0 + (1 - F0) * pow(1 - dot(o, h), 5);

    // brdf
    float fr= (F * D * G2) / (4 * cosTheta_o * cosTheta);


    color = fr * diffuse * cosTheta;//* diffuse * cosTheta;
    #endif


    // transparency
    if(baseColorTex.a < 0.4) discard;

    fragment_color  = baseColorTex * vec4(color , 1.0);   
}

