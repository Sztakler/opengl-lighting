#version 330 core
layout (location = 0) in vec3 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 centerPosition;

void main()
{
    gl_Position = projection * view * model * vec4(aPosition + centerPosition, 1.0);
}

