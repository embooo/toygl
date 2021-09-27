#version 430 core

out vec4 fragment_color;

in vec3 pNormal;
in vec3 p; // position du point de la surface dans le rep�re camera
in mat4 viewMat; // passage du rep�re monde au rep�re camera

// Lumi�re ponctuelle
uniform vec4 cLight; // couleur de la lumi�re 
uniform vec3 pLight; // position de la lumi�re dans le rep�re monde
uniform vec3 pCam;  // position de la camera dans le rep�re monde

const float PI = 3.14;

#define LAMBERT

void main( )
{
    // Color
    vec3 posLight = vec3 ( viewMat * vec4(pLight, 1.0) ) + vec3(0, 0, 0);
    vec4 baseColor = vec4(0.9, 0.9, 0.9, 1.0);
    vec4 color = baseColor;

    // surface normal
    vec3 N = normalize(pNormal);
    // directions
    vec3 O = normalize(vec3( (viewMat *  vec4(pCam, 1.0))   - (viewMat * vec4(p, 1.0)) ) ); // observateur 
    vec3 L = normalize(vec3( (viewMat *  vec4(pLight, 1.0)) - (viewMat * vec4(p, 1.0)) )); // lumi�re 

    float cosTheta = dot(N, L) ; // angle

#ifdef LAMBERT
    // Modele de lambert
    float k = 0.9;
    float fr = k / PI; // fonction de reflection 

    color *= fr * cosTheta * cLight;
#endif


#ifdef BLINN
// bissectrice
    vec3 H = normalize(O + L);
    
    float m = 1.0;
    float cosTheta_H = dot(N, H);

    float fr = ( (m + 8) / (8 * PI)  ) * pow(cosTheta_H, m);

    color *= fr * cosTheta * cLight;
#endif
    
    fragment_color = color;   
}

