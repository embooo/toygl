#version 330 core

in vec4 vPosition;
in vec3 vNormal;

uniform vec4 cameraPosition;

out vec4 FragColor;

const float PI = 3.14;

void main()
{
    // Lambert
    float k = 1.0;
    float reflectanceFunction = k / PI;

    vec4 baseColor  = vec4(0.8, 0.8, 0.8, 1.0);
    vec4 lightColor = vec4(0.2, 0.1, 0.23, 1.0);
    vec3 lightPosition = vec3(0.0, 10.0, 0.0);

    vec3 lightDir =  lightPosition - vec3(vPosition);

    float cosTheta = dot(normalize(vNormal), normalize(lightDir)) / (length(vNormal) * length(lightDir));

    gl_FragColor = baseColor * (reflectanceFunction * cosTheta * lightColor);
}