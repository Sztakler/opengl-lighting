#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aOffset;
layout (location = 2) in vec3 angles;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;

out vec3 color;

void main()
{   
    float r_sin = sin(angles.x);
    float r_cos = cos(angles.x);

    mat3 rotation_x = mat3( 1.f, 0.f, 0.f,
                            0.f, r_cos, -r_sin,
                            0.f, r_sin, r_cos);

    mat3 rotation_y = mat3( r_cos, 0, r_sin,
                            0, 1, 0,
                            -r_sin, 0, r_cos);

    mat3 rotation_z = mat3( r_cos, -r_sin, 0,
                            r_sin, r_cos, 0,
                            0, 0, 1);

    mat3 rotation_zyx = rotation_z * rotation_y * rotation_x;


    // mat3 rotation_zyx = rotation_x * rotation_y * rotation_z; 


    vec3 pos = rotation_zyx * aPosition;
    vec4 position = vec4(pos + aOffset * 2.0, 1.0);

    gl_Position = projection * view * model * position;
    
    color = vec3((aOffset.x) / 2.0, (aOffset.y) / 2.0, (aOffset.z ) / 2.0);
}