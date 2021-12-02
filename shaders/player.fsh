#version 330 core
out vec4 FragColor;
in vec3 objectColor;
in vec3 normal;
in vec3 fragmentPosition;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform vec3 viewerPosition;

void main()
{
    /* Normalizing vectors */
    vec3 _normal = normalize(normal);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    
    /* Calculating ambient lighting */
    float ambientStrenght = 0.1f;
    vec3 ambientLight = ambientStrenght * lightColor;

    /* Calculating diffuse lighting*/
    float diffuseStrenght = max( dot(_normal, lightDirection), 0.0f );
    vec3 diffuseLight = diffuseStrenght * lightColor;

    /* Calculating specular lighting */
    float specularStrenght = 0.5;
    vec3 viewDirection = normalize(viewerPosition - fragmentPosition);
    vec3 reflectionDirection = reflect(-lightDirection, _normal); // we take negative lightDirection vector,
                                                                  // because it's pointing the other way 
                                                                  // than we want
    float specularValue = pow( max( dot(viewDirection, reflectionDirection), 0.0), 64 );
    vec3 specularLight = specularStrenght * specularValue * lightColor;

    vec3 resultColor = (ambientLight + diffuseLight + specularLight) * objectColor;

    FragColor = vec4(resultColor, 1.0f);
}