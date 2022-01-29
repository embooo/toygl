#version 430 core

// Varyings
out vec4 fragment_color;

in mat4 modelMat;
in mat4 viewMat;

in vec3 p; 
in vec3 n; 
in vec4 t; 
in vec3 b; 
flat in int matId; 
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

// Toggles
uniform bool enableNormalMap;
uniform bool enableGammaCorrection;

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

vec3 fresnelSchlick(vec3 F0, float cosTheta)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
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
uniform vec3  lightDir;

uniform sampler2DArray textureArray;

struct Material
{
    int albedo;
    int normal;
    int specular;
    int emissive;
    int occlusion;
    int metalRough;

//    float metallicFactor;
//    float roughnessFactor;
//
//    vec3 emissiveFactor;
//    vec4 baseColorFactor;
};

layout(std430, binding = 0) buffer MaterialData
{
    Material mats[];
};

void main()
{
    vec3 N   = normalize(n); 

    // Read textures
    vec4 diffuse           = texture(textureArray, vec3(texcoord[0], mats[matId].albedo ));
    vec4 normal            = texture(textureArray, vec3(texcoord[0], mats[matId].normal ));
    vec4 roughnessMetallic = texture(textureArray, vec3(texcoord[0], mats[matId].metalRough));
    vec4 emissive          = texture(textureArray, vec3(texcoord[0], mats[matId].emissive));
    vec4 occlusion         = texture(textureArray, vec3(texcoord[0], mats[matId].occlusion));
    vec4 specular          = texture(textureArray, vec3(texcoord[0], mats[matId].specular));

    if(enableNormalMap)
    {
       // Build the matrix to convert tangeant normal to world space 
       vec3 T = normalize(vec3(modelMat * vec4(t.xyz,   0.0)));
       vec3 B = normalize(cross(N, T))  * t.w; // don't forget handedness
            N = normalize(vec3(modelMat * vec4(n,    0.0)));

       mat3 TBN = mat3(T, B, N);

       // World space normal
       N = TBN * (normal.rgb * 2 - 1);
       N = normalize(N);
    }

    // https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-24-importance-being-linear
    if(enableGammaCorrection)
    {
        // Manually convert color values to linear space
        diffuse.rgb = pow(diffuse.rgb, vec3(2.2));
    }


    // Metallic + roughness
    // https://www.khronos.org/blog/art-pipeline-for-gltf
    float roughness = roughnessMetallic.g; 
    float metallic  = roughnessMetallic.b; 

    DirectionalLight dl;
    dl.color     = lightColor.rgb ;
    dl.direction = (normalize(modelMat * vec4(lightDir, 1.0) )).xyz;

    PointLight pl;
    pl.color     = lightColor.rgb;
    pl.pos       = (vec4(lightPos, 1.0) * modelMat).xyz  ;
//  pl.pos       = (modelMat * vec4(lightPos, 1.0)).xyz  ;
    pl.radius    = lightRadius;

    // Point light attenuation
//    vec3 L            = pl.pos - p; // point light
    vec3 L            = -dl.direction; // directinal light
    float attenuation = 1 / (pow(length(L), 2) + pow(pl.radius, 2) / 2);

    // Directions
    vec3 camPos = cameraPos;
    vec3 o      = normalize(camPos - p); // view direction 
    vec3 l      = normalize(L);
    vec3 h      = normalize(o+l);
    
    float cosTheta    = max(0, dot(N, l));
    float cosTheta_h  = max(0, dot(N, h));  // specular intensity
    float cosTheta_o  = max(0, dot(N, o));
    float cosTheta_oh = max(0, dot(o, h));

    // Cook-Torrance BRDF
    float shininess = roughnessMetallic.y;

    float fr   = (D(roughness, cosTheta_h) * F(cosTheta_oh, 0.1) * G(roughness, cosTheta, cosTheta_o)) / (4 * cosTheta * cosTheta_o);


    float frBlinnPhong = ((shininess + 8) / (8*M_PI)) * pow(cosTheta_h, shininess);

    vec3 F0 = mix(vec3(0.04), diffuse.rgb, metallic);

    vec3 F  = fresnelSchlick(F0, max(0.0, dot(h, o)));

    vec3 kd = mix(vec3(1.0) - F, vec3(0.0), metallic);

//    vec3  frLambert = 1.0 * diffuse.rgb;
    vec3  frLambert = diffuse.rgb / M_PI;

//    vec3 diffuseColor  = frLambert.rgb * dl.color * cosTheta * attenuation; // point light
//    vec3 diffuseColor  = frBlinnPhong * dl.color * cosTheta;
    vec3 diffuseColor   = frLambert * dl.color * cosTheta;
    vec3 specularColor  = fr * dl.color * cosTheta;
    
    vec4 color = vec4(diffuseColor, 1) * vec4(specularColor, 1);
//         color = color / (1.0f + color);

    
    if(enableGammaCorrection)
    {
        // apply gamma correction to linear space color
        color.rgb = pow(color.rgb, vec3(1.0/2.2));
    }

    if(diffuse.a < 0.3) discard;

    fragment_color = color;
}



