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

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outCutOff;

};

uniform Material material;
uniform Light light;

void main()
{
    // vec3 lightColor = (1.0, 1.0, 1.0);
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(light.position - fragmentPosition);

    float distance = length(light.position - fragmentPosition);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    /* Beer-Lambert Law https://graphics.fandom.com/wiki/Beer-Lambert_law */
    float beer_intensity = pow(10.0, -light.linear * light.quadratic * distance * 2);
    
    float theta = dot(lightDirection, normalize(-light.direction));
    float epsilon = light.cutOff -light.outCutOff;
    float intensity = clamp( (theta - light.outCutOff) / epsilon, 0.0, 1.0);


    vec3 ambient = material.ambient * light.ambient; 

    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;

    vec3 viewDirection = normalize(viewerPosition - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), material.shininess);
    vec3 specular = (material.specular * spec) * light.specular;

    diffuse *= intensity;
    specular *= intensity;

    vec3 result = (ambient + diffuse + specular + vec3(0.0314, 0.1686, 0.4275)) * beer_intensity ;
    // vec3 result = (ambient + diffuse + specular) * attenuation;
    FragColor = vec4(result, 1.0);
} 