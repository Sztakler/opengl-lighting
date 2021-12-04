#version 330 core
out vec4 FragColor;

in vec3 objectColor;
in vec3 fragmentPosition;  
in vec3 normal;  
  
uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewerPosition; 

// struct Material
// {
//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
//     float shininess;
// };

// struct Light
// {
//     vec3 position; // only necessary when using point light
//     // vec3 direction; // necessary when using directional light

//     vec3 ambient;
//     vec3 diffuse;
//     vec3 specular;
// };

// uniform Material material;
// uniform Light light;

void main()
{
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
        
    vec3 _normal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    // vec3 lightDirection = normalize(-light.direction);

    float diffuseStrenght = max( dot(_normal, lightDirection), 0.0f );
    vec3 diffuse = diffuseStrenght * lightColor;

    float specularStrenght = 0.5f;
    vec3 viewDirection = normalize(viewerPosition - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, _normal);

    float spec = pow( max( dot(viewDirection, reflectionDirection), 0.0f), 32);
    vec3 specular = (specularStrenght * spec) * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 