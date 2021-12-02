#include "player.h"

Player::Player(const char* vertices_data_filename, const char* normals_data_filename,
               const char* vertex_shader_filename, const char* fragment_shader_filename) 
{

    loadData(vertices_data_filename, this->vertices, 1.0);
    loadData(normals_data_filename, this->normals, 1.0);

    this->vertices_array = VAO();
    this->vertices_array.Bind();
    this->vertices_buffer = VBO(&this->vertices, this->vertices.size() * sizeof(float));

    this->normals_array = VAO();
    this->normals_array.Bind();
    this->normals_buffer = VBO(&this->normals, this->normals.size() * sizeof(float));

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);
    
    this->vertices_array.link_vbo(this->vertices_buffer, 0, 3);
    this->normals_array.link_vbo(this->normals_buffer, 1, 3);

}

void Player::Bind()
{
    this->vertices_array.Bind();
    this->normals_array.Bind();
}

void Player::Unbind()
{
    this->vertices_array.Unbind();
    this->normals_array.Unbind();
}

void Player::Draw()
{
    glDrawArrays(GL_TRIANGLE_FAN, 0,  vertices.size() / 3);
}

void Player::Draw(glm::mat4* model, glm::mat4* view, glm::mat4* projection, DRAWING_MODE drawing_mode)
{
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(*model));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(*projection));

    switch (drawing_mode)
    {
        case TRIANGLES:
            glDrawArrays(GL_TRIANGLES, 0,  vertices.size() / 3);
            break;

        case WIREFRAME:
            glDrawArrays(GL_LINES, 0,  vertices.size() / 3);
            break;
        default:
            glDrawArrays(GL_TRIANGLES, 0,  vertices.size() / 3);
    }
    
}

void Player::loadData(const char* filename, std::vector<float> &data, float scale)
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