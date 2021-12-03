#include "player.h"

Player::Player(const char* vertices_data_filename, const char* normals_data_filename,
               const char* vertex_shader_filename, const char* fragment_shader_filename) 
{

    // loadData(vertices_data_filename, this->vertices, 1.0);
    loadFromObjectFile("/home/krystian-jasionek/Studia/Semestr V/PGK/opengl-lighting/data/suzanne.obj");
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

bool Player::loadFromObjectFile(const char* filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        return false;
    
    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec3> temp_normals;


    while(!f.eof())
    {
        char line[128];
        f.getline(line, 128);

        std::stringstream s;
        s << line;
        

        char junk;

        if (line[0] == 'v')
        {
            if (line[1] == 'n')
            {
                glm::vec3 vn;
                s >> junk >> vn.x >> vn.y >> vn.z;
                temp_normals.push_back(vn);
            }
            else
            {
                glm::vec3 v;
                s >> junk >> v.x >> v.y >> v.z;
                temp_vertices.push_back(v);
            }
        }

        if (line[0] == 'f')
        {
            int f[3];
            s >> junk >> f[0] >> f[1] >> f[2];
            for (int i = 0; i < 3; i++)
            {
            this->vertices.push_back(temp_vertices[f[i] - 1].x);
            this->vertices.push_back(temp_vertices[f[i] - 1].y);
            this->vertices.push_back(temp_vertices[f[i] - 1].z);
            }
        }

        if (line[0] == 'n')
        {
            int n[3];
            s >> junk >> n[0] >> n[1] >> n[2];
            for (int i = 0; i < 3; i++)
            {
            this->normals.push_back(temp_normals[n[i] - 1].x);
            this->normals.push_back(temp_normals[n[i] - 1].y);
            this->normals.push_back(temp_normals[n[i] - 1].z);
            }
        }
    }

    for (int i = 0; i < this->vertices.size(); i += 3)
    {
        std::cout << "(" << this->vertices[i] << ", " << this->vertices[i + 1] << ", " << this->vertices[i + 2] << ")\n";
    }

    return true;
}