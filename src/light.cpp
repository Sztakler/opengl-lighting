#include "light.h"

Light::Light(const char* obj_data_filename, const char* vertex_shader_filename,
             const char* fragment_shader_filename, 
             glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : Drawable(obj_data_filename, 
             vertex_shader_filename, fragment_shader_filename)
    {
        this->position = glm::vec3(0.0, 0.0, 0.0);
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->specular = specular;
    }