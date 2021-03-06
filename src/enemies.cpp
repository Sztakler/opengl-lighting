#include "enemies.h"

Enemies::Enemies(const char* obj_data_filename,
    const char* vertex_shader_filename, const char* fragment_shader_filename,
    int seed, glm::vec3 boardsize, Material material, int n_enemies)
{
    // printf("lalala\n");
    // this->translations = std::vector<float>(n_enemies * 3);
    // this->positions    = std::vector<glm::vec3>(n_enemies);
    // this->velocities   = std::vector<float>(n_enemies);
    
    srand(seed);

    loadFromObjectFile(obj_data_filename);

    for (int i = 0; i < n_enemies * 3; i += 3)
    {
        this->translations.push_back(rand() % 2000 / 100.0);
        this->translations.push_back(rand() % 2000 / 100.0);
        this->translations.push_back(rand() % 3800 / 100.0 + 5.0);
    }


    // printf("COLORS:\n");
    for (int i = 0; i < n_enemies; i++)
    {
        this->velocities.push_back(rand() % 100 / 1000);
        this->positions.push_back(glm::vec3( this->translations[3 * i],
                                             this->translations[3 * i + 1], 
                                             this->translations[3 * i + 2] ));
                                             
        glm::vec3 color = glm::vec3(rand() % 1000 / 1000.0, rand() % 1000 / 1000.0, rand() % 1000 / 1000.0);

        this->colors.push_back(color.x);
        this->colors.push_back(color.y);
        this->colors.push_back(color.z);

        this->colors_vec.push_back(color);

        // printf("%f, %f, %f\n", color.x, color.y, color.z);
    }

    this->vertices_array = VAO();
    this->vertices_array.Bind();
    this->vertices_buffer = VBO(&this->vertices, this->vertices.size() * sizeof(float));

    this->normals_array = VAO();
    this->normals_array.Bind();
    this->normals_buffer = VBO(&this->normals, this->normals.size() * sizeof(float));

    this->translations_array = VAO();
    this->translations_array.Bind();
    this->translations_buffer = VBO(&this->translations, this->translations.size() * sizeof(float));

    this->colors_array = VAO();
    this->colors_array.Bind();
    this->colors_buffer = VBO(&this->colors, this->colors.size() * sizeof(float));

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);

    this->vertices_array.link_vbo(this->vertices_buffer, 0, 3);
    this->normals_array.link_vbo(this->normals_buffer, 1, 3);
    this->translations_array.link_instance_vbo(this->translations_buffer, 2, 3, 1);
    this->colors_array.link_instance_vbo(this->colors_buffer, 3, 3, 1);
    // this->velocities_array.link_instance_vbo(this->velocities_buffer, 3, 1, 1);

    this->material.ambient = material.ambient;
    this->material.diffuse = material.diffuse;
    this->material.specular = material.specular;
    this->material.shininess = material.shininess;

    this->yOffset = 0.0;
    this->velocity = 0.01;
    // this->position = glm::vec3(0.0, 0.0, 0.0);
}

void Enemies::Bind()
{
    this->vertices_array.Bind();
    this->normals_array.Bind();
    this->translations_array.Bind();
    this->colors_array.Bind();
}

void Enemies::Unbind()
{
    this->vertices_array.Unbind();
    this->normals_array.Unbind();
    this->translations_array.Unbind();
    this->colors_array.Unbind();
}

void Enemies::Draw()
{
    glDrawArrays(GL_TRIANGLES, 0,  vertices.size() / 3);
}

void Enemies::DrawInstanced(GLsizei primcount, glm::mat4* model, glm::mat4* view, glm::mat4* projection, DRAWING_MODE drawing_mode, bool transparent, glm::vec3 camera_position)
{   
    if (transparent)
    {
        sortPositions(camera_position);
        this->colors_buffer.Update(&this->colors);
    }
    this->translations_buffer.Update(&this->translations);

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

void Enemies::loadUniforms()
{   
    glUniform1f(glGetUniformLocation(shader.id, "yOffset"), this->yOffset);
    glUniform3fv(glGetUniformLocation(shader.id, "material.ambient"),  1, glm::value_ptr(material.ambient));
    glUniform3fv(glGetUniformLocation(shader.id, "material.diffuse"),  1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(shader.id, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f (glGetUniformLocation(shader.id, "material.shininess"),   material.shininess);
}

void Enemies::sortPositions(glm::vec3 camera_position)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> position_color_vector;
    for (int i = 0; i < positions.size(); i++)
    {
        std::pair<glm::vec3, glm::vec3> p(positions[i], colors_vec[i]);
        position_color_vector.push_back(p);
    }

    std::sort(std::begin(position_color_vector), std::end(position_color_vector),
          [camera_position](const std::pair<glm::vec3, glm::vec3> &a, const std::pair<glm::vec3, glm::vec3> &b) {
            return glm::length(a.first - camera_position) > glm::length(b.first - camera_position);
          });

    for (int i = 0; i < position_color_vector.size(); i++)
    {
        this->translations[3 * i]     = position_color_vector[i].first.x;
        this->translations[3 * i + 1] = position_color_vector[i].first.y;
        this->translations[3 * i + 2] = position_color_vector[i].first.z;

        // printf("%d %f, %f, %f\n", i, position_color_vector[i].first.x, position_color_vector[i].first.y, position_color_vector[i].first.z);

        this->positions[i]            = position_color_vector[i].first;

        this->colors[3 * i]           = position_color_vector[i].second.x;
        this->colors[3 * i + 1]       = position_color_vector[i].second.y;
        this->colors[3 * i + 2]       = position_color_vector[i].second.z;

        this->colors_vec[i]           = position_color_vector[i].second;
    }
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
}

bool Enemies::loadFromObjectFile(const char* filename)
{
    std::ifstream f(filename);
    if (!f.is_open())
        return false;
    
    std::vector<vec3d> temp_vertices;
    std::vector<vec3d> temp_normals;
    vec3d vn = {0.0, 0.0, 0.0};
    vec3d v = {0.0, 0.0, 0.0};

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
                char delimiter[] = " ";
                std::vector<std::string> tokens;

                char* token = strtok(line, delimiter);
                while (token)
                {
                    tokens.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                vn.x = stof(tokens[1]);
                vn.y = stof(tokens[2]);
                vn.z = stof(tokens[3]);

                temp_normals.push_back(vn);
            }
            else
            {
                char delimiter[] = " ";
                std::vector<std::string> tokens;

                char* token = strtok(line, delimiter);
                while (token)
                {
                    tokens.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                v.x = stof(tokens[1]);
                v.y = stof(tokens[2]);
                v.z = stof(tokens[3]);

                temp_vertices.push_back(v);
            }
        }

        if (line[0] == 'f')
        {
            int f[3];
            int n[3];

            std::string parts[3];

            s >> junk >> parts[0] >> parts[1] >> parts[2];
            
            for (int i = 0; i < 3; i++)
            {
                replace(parts[i], "//", " ");

                char delimiter[] = " ";
                std::vector<std::string> numbers;

                char* token = strtok(const_cast<char*>(parts[i].c_str()), delimiter);
                while (token != nullptr)
                {
                    numbers.push_back(std::string(token));
                    token = strtok(nullptr, delimiter);
                }

                f[i] = stoi(numbers[0]);
                n[i] = stoi(numbers[1]);
            }

            for (int i = 0; i < 3; i++)
            {
                this->vertices.push_back(temp_vertices[f[i] - 1].x);
                this->vertices.push_back(temp_vertices[f[i] - 1].y);
                this->vertices.push_back(temp_vertices[f[i] - 1].z);

                this->normals.push_back(temp_normals[n[i] - 1].x);
                this->normals.push_back(temp_normals[n[i] - 1].y);
                this->normals.push_back(temp_normals[n[i] - 1].z);
            }

        }
    }

    return true;
}

bool Enemies::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}