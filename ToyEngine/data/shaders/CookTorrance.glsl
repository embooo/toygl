#version 430 core

// Varyings
out vec4 fragment_color;

in mat4 modelMat;
in mat4 viewMat;

in vec3 p; 
in vec3 n; 
in vec3 t; 
in vec3 b; 
in vec2 texcoord[2];

// Uniforms
uniform vec3 cameraPos;

// Textures
uniform sampler2D baseColorTexture;
uniform vec4      baseColorFactor;
uniform int       currentBaseColorTexcoord;

uniform float     roughnessFactor;
uniform sampler2D metallicRoughnessTexture;
uniform int       currentMetallicRoughnessTexcoord;

uniform sampler2D normalTexture;
uniform int       currentNormalTexcoord;

uniform sampler2D emissiveTexture;
uniform vec3      emissiveFactor;

const float M_PI = 3.141592;

// Cook-Torrance model
float D(float alpha, float cosThetaH);                  // Normal Distribution Function
float G(float cosThetaOH);                              // Geometry attenuation
float F(float alpha, float cosTheta, float cosThetaO);  // Fresnel


float D(float alpha, float cosThetaH)
{
    // GGX Normal Distribution Function [Trowbridge-Reitz 1975]
    // Tells us how many microfacets are oriented in the halfway vector direction H (producing a mirror reflection)
    // Reflection is more intense the bigger the amount of microfacets oriented towards H
    
//    float alpha_Sq     = alpha*alpha;
//    float cosThetaH_Sq = cosThetaH*cosThetaH;
//    float d = M_PI * (cosThetaH_Sq * (alpha_Sq - 1) + 1);
//
//    return alpha_Sq / (d*d);

    float D= alpha / (1 + cosThetaH*cosThetaH * (alpha*alpha -1));
    D= D*D / M_PI;

    return D;
}

float G(float alpha, float cosTheta, float cosThetaO)
{
    // Contributes to normalizing the microfacet distribution to guarantee energy conservation 

    // Remapping
//    alpha = (alpha + 1.0) / 2.0;
//
//    float k = ((alpha + 1) * (alpha + 1))/8;
//
//    float G1  = cosTheta / (cosTheta * (1 - k) + k); // Light direction
//    float G1o = cosThetaO / (cosThetaO * (1 - k) + k); // View direction
//
//    return G1 * G1o;

    float tan2_theta_o= 1 / (cosThetaO*cosThetaO) - 1;
    float lambda_o= 1 + alpha*alpha * tan2_theta_o;
    float tan2_theta= 1 / (cosTheta*cosTheta) - 1;
    float lambda= 1 + alpha*alpha * tan2_theta;
    float G2= 2 / (sqrt(lambda_o) + sqrt(lambda));

    return G2;
}

float F(float cosThetaOH, float n2)
{
    // Schlick fresnel approximation
    // n2 : refraction index of the object
    // F0 : reflection coefficient
    float F0 = (1.0 - n2) / (1.0 + n2); 

    return F0 + (1 - F0) * pow(2, (-5.55473 * cosThetaOH-6.98316)*cosThetaOH);
}


// Types of light casters
struct PointLight
{
    vec3  pos;
    vec3  color;
    float radius;
};

struct DirectionalLight
{
    vec3 direction;
    vec3 color;
};


// TODO : Remove 
uniform float lightRadius;
uniform vec3  lightPos;
uniform vec4  lightColor;

void main()
{
    vec3 N   = normalize(n); 
    vec3 B   = normalize(b);
    vec3 T   = normalize(cross(B, N));

    mat3 tbn = mat3(T, B, N);

    // Colors
    vec4 diffuse           = texture(baseColorTexture, texcoord[currentBaseColorTexcoord]);
    if(diffuse.a < 0.8) discard;
    
    vec4 roughnessMetallic = texture(metallicRoughnessTexture, texcoord[currentMetallicRoughnessTexcoord]);
    vec4 emissive          = texture(emissiveTexture,          texcoord[0]);
    vec4 normal            = texture(normalTexture,            texcoord[currentNormalTexcoord]); // tangeant space normalTexture

    N = normal.xyz * 2 - 1;
    N = normalize(tbn * N);

    // Metallic + roughness
    float r = roughnessMetallic.y; 
    float m = roughnessMetallic.z; 

    DirectionalLight dl;
    dl.color     = vec3(1,1,1) ;
    dl.direction = vec3(-0.5, -0.5, -1);

    PointLight pl;
    pl.color     = vec3(lightColor);
    pl.pos       = (modelMat * vec4(lightPos, 1.0)).xyz;
    pl.radius    = lightRadius;

    // Point light attenuation
    vec3 L            = pl.pos - p;
    float attenuation = 1 / (pow(length(L), 2) + pow(pl.radius, 2) / 2);

    // Directions
    vec3 camPos = vec3(modelMat * vec4(cameraPos, 1));
    vec3 o      = normalize(-p); // view direction 
    vec3 l      = normalize(L);
    vec3 h      = normalize(o+l);
    
    float cosTheta    = max(0, dot(N, l));
    float cosTheta_h  = max(0, dot(N, h));  // specular intensity
    float cosTheta_o  = max(0, dot(N, o));
    float cosTheta_oh = max(0, dot(o, h));

    // Cook-Torrance BRDF
    float fr   = (D(r, cosTheta_h) * F(cosTheta_oh, 0.1) * G(r, cosTheta, cosTheta_o)) / (4 * cosTheta * cosTheta_o);
    float frLambert    = 1.0 / M_PI;
    float shininess    = roughnessMetallic.y;

    float frBlinnPhong = ((shininess + 8) / (8*M_PI)) * pow(cosTheta_h, shininess);

    vec3 diffuseColor  = fr * pl.color * cosTheta * attenuation;
    
    vec4 color = vec4(diffuseColor, 1) * diffuse;
         color = color / (1.0f + color);

    fragment_color = color ;
//    fragment_color = lightColor ;
}



