#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require
out vec4 FragColor;

in vec3 objectColor;
in vec3 fragmentPosition;  
in vec3 Normal;  
  
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewerPosition; 

void main()
{
    // vec3 lightColor = (1.0, 1.0, 1.0);
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    
    float ambientStrenght = 0.1;
    vec3 ambient = ambientStrenght * lightColor; 

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * lightColor;


    vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
} 