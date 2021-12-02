#include "enemies.h"

Enemies::Enemies(const char* vertices_data_filename, const char* colors_data_filename,
    const char* vertex_shader_filename, const char* fragment_shader_filename, int seed, int boardsize)
{
    loadData(vertices_data_filename, this->vertices, 0.15);

    int index = 0;
    float offset = 0.1f;
    glm::vec3 euler_angles;
    AABB new_aabb;

    srand(seed);

    for(int y = 0; y < 2 * boardsize; y += 2)
    {
        for(int x = 0; x < 2 * boardsize; x += 2)
        {
            for (int z = 0; z < 2 * boardsize; z += 2)
            {
                glm::vec3 translation;
                glm::vec3 rotation;

                translation = glm::vec3( (float)x / (float)boardsize + offset,
                                         (float)y / (float)boardsize + offset,
                                         (float)z / (float)boardsize + offset );
                translations.push_back(translation.x);
                translations.push_back(translation.y);
                translations.push_back(translation.z);

                rotation = glm::vec3( glm::radians((float)(rand() % 180)),
                                      glm::radians((float)(rand() % 180)),
                                      glm::radians((float)(rand() % 180)) );
                rotations.push_back(rotation.x);
                rotations.push_back(rotation.y);
                rotations.push_back(rotation.z);

                float r_sin = sin(rotation.x);
                float r_cos = cos(rotation.x);

                glm::mat3 rotation_x = glm::mat3( 1.f, 0.f, 0.f,
                                                  0.f, r_cos, -r_sin,
                                                  0.f, r_sin, r_cos);

                glm::mat3 rotation_y = glm::mat3( r_cos, 0, r_sin,
                                                  0, 1, 0,
                                                  -r_sin, 0, r_cos);

                glm::mat3 rotation_z = glm::mat3( r_cos, -r_sin, 0,
                                                  r_sin, r_cos, 0,
                                                  0, 0, 1);

                glm::mat3 rotation_zyx = rotation_z * rotation_y * rotation_x;

                std::vector<glm::vec3> enemy_vertices;
                glm::vec3 vertex_coordinates;

                translation.x *= 2;
                translation.y *= 2;
                translation.z *= 2;

                for (int i = 0; i < 4; i++)  // dla każdego wierzchołka czworościanu
                {   
                    // współrzędne wierzchołka
                    vertex_coordinates = glm::vec3( vertices[i * 3],
                                                    vertices[i * 3 + 1],
                                                    vertices[i * 3 + 2] );


                    // printf("TRANSLATION %f, %f, %f\n", translation.x, translation.y, translation.z);

                    glm::vec3 pos = rotation_zyx * vertex_coordinates;
                    glm::vec3 position = pos + translation;

                    enemy_vertices.push_back(position);
                }



                new_aabb.Bminx = std::min( {enemy_vertices[0].x, enemy_vertices[1].x, enemy_vertices[2].x, enemy_vertices[3].x} );
                new_aabb.Bmaxx = std::max( {enemy_vertices[0].x, enemy_vertices[1].x, enemy_vertices[2].x, enemy_vertices[3].x} );
                
                new_aabb.Bminy = std::min( {enemy_vertices[0].y, enemy_vertices[1].y, enemy_vertices[2].y, enemy_vertices[3].y} );
                new_aabb.Bmaxy = std::max( {enemy_vertices[0].y, enemy_vertices[1].y, enemy_vertices[2].y, enemy_vertices[3].y} );

                new_aabb.Bminz = std::min( {enemy_vertices[0].z, enemy_vertices[1].z, enemy_vertices[2].z, enemy_vertices[3].z} );
                new_aabb.Bmaxz = std::max( {enemy_vertices[0].z, enemy_vertices[1].z, enemy_vertices[2].z, enemy_vertices[3].z} );

                // printf("AABB %f, %f, %f, %f, %f, %f\n", new_aabb.Bminx, new_aabb.Bmaxx, new_aabb.Bminy, new_aabb.Bmaxy, new_aabb.Bminz, new_aabb.Bmaxz);

                this->enemies_boxes.push_back(new_aabb);
            }
        }
    }  

    this->vertices_array = VAO();
    this->vertices_array.Bind();
    this->vertices_buffer = VBO(&this->vertices, this->vertices.size() * sizeof(float));

    // std::cout << "VERTICES" << std::endl;
    // for (float p : this->vertices)
    // {
    //     std::cout << p << " ";
    // }
    // std::cout << std::endl;

    this->translations_array = VAO();
    this->translations_array.Bind();
    this->translations_buffer = VBO(&this->translations, this->translations.size() * sizeof(float));

    this->rotations_array = VAO();
    this->rotations_array.Bind();
    this->rotations_buffer = VBO(&this->rotations, this->rotations.size() * sizeof(float));

    // std::cout << "\nTRANSLATIONS" << std::endl;
    // for (float t : this->translations)
    // {
    //     std::cout << t << " ";
    // }
    // std::cout << std::endl;

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);
    
    this->vertices_array.link_vbo(this->vertices_buffer, 0, 3);
    this->translations_array.link_instance_vbo(this->translations_buffer, 1, 3, 1);
    this->rotations_array.link_instance_vbo(this->rotations_buffer, 2, 3, 1);
}

void Enemies::Bind()
{
    this->vertices_array.Bind();
    this->translations_array.Bind();
    this->rotations_array.Bind();
}

void Enemies::Unbind()
{
    this->vertices_array.Unbind();
    this->translations_array.Unbind();
    this->rotations_array.Unbind();
}

void Enemies::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0,  vertices.size() / 3);
}

void Enemies::DrawInstanced(GLsizei primcount, glm::mat4* model, glm::mat4* view, glm::mat4* projection, DRAWING_MODE drawing_mode)
{
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(*model));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(*projection));

    switch (drawing_mode)
    {
        case TRIANGLES:
            glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size() / 3, primcount);
            break;

        case WIREFRAME:
            glDrawArraysInstanced(GL_LINES, 0, vertices.size() / 3, primcount);
            break;
        default:
            glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size() / 3, primcount);
    }

    
}

void Enemies::DrawInstanced(GLsizei primcount)
{
    glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size() / 3, primcount);
}

void Enemies::loadData(const char* filename, std::vector<float> &data, float scale)
{
    std::ifstream in(filename);
    std::string line;

    while(std::getline(in, line))
    {
        std::string value; 
        for(auto &c : line)
        {
            if (c == ',')
            {
                data.push_back(std::stof(value) * scale);
                value = "";
                continue;
            }
            value += c;
        }
    }

    // for(float value : data)
    // {
    //     std::cout << value << " ";
    // }
    // std::cout << std::endl;

}