#ifndef LIGHT_H
#define LIGHT_H

#include "includes.h"
#include "drawable.h"

class Light : public Drawable
{
    public:
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;

    public:
        Light(const char* obj_data_filename, const char* vertex_shader_filename,
              const char* fragment_shader_filename, 
              glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
};

#endif