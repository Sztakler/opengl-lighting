#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform vec3 centerPosition;

out vec3 objectColor;
out vec3 fragmentPosition;
out vec3 normal;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition + centerPosition, 1.0);
    objectColor = vec3(0.3f, 0.3f, 0.3f);
    fragmentPosition = vec3(model * vec4(aPosition + centerPosition, 1.0f));
    normal = aNormal;
}
