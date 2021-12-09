#include "drawable_light.h"

DrawableLight::DrawableLight(const char* vertices_data_filename, const char* normals_data_filename,
               const char* vertex_shader_filename, const char* fragment_shader_filename) 
{

    loadData(vertices_data_filename, this->vertices, 1.0);
    // loadFromObjectFile("data/plane.obj");
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

DrawableLight::DrawableLight(const char* obj_data_filename, const char* vertex_shader_filename,
               const char* fragment_shader_filename, int seed) 
{

    loadFromObjectFile(obj_data_filename);
    srand(seed);

    this->vertices_array = VAO();
    this->vertices_array.Bind();
    this->vertices_buffer = VBO(&this->vertices, this->vertices.size() * sizeof(float));

    this->normals_array = VAO();
    this->normals_array.Bind();
    this->normals_buffer = VBO(&this->normals, this->normals.size() * sizeof(float));

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);
    
    this->vertices_array.link_vbo(this->vertices_buffer, 0, 3);
    this->normals_array.link_vbo(this->normals_buffer, 1, 3);

    this->point_light.position = glm::vec3(rand() % 2000 / 100.0, rand() % 2000 / 100.0, rand() % 3500 / 100.0 + 5.0);
    this->point_light.constant = 0.2;
    this->point_light.linear = 0.09;
    this->point_light.quadratic = 0.036;

    glm::vec3 color(rand() % 2000 / 2000.0, rand() % 2000 / 2000.0, rand() % 4000 / 4000.0);

    this->material.ambient = glm::vec3(color.x / 4, color.y / 4, color.z / 4);
    this->material.diffuse = glm::vec3(color.x, color.y, color.z);
    this->material.specular = glm::vec3(color.x * 2, color.y * 2, color.z * 2);
    this->material.shininess = 36.0;

    this->point_light.ambient = glm::vec3(color.x / 4, color.y / 4, color.z / 4);
    this->point_light.diffuse = glm::vec3(color.x, color.y, color.z);
    this->point_light.specular = glm::vec3(color.x * 2, color.y * 2, color.z * 2);

    this->position = this->point_light.position;
}

DrawableLight::DrawableLight(const char* obj_data_filename, const char* vertex_shader_filename,
               const char* fragment_shader_filename, PointLight point_light)
{
    loadFromObjectFile(obj_data_filename);

    this->vertices_array = VAO();
    this->vertices_array.Bind();
    this->vertices_buffer = VBO(&this->vertices, this->vertices.size() * sizeof(float));

    this->normals_array = VAO();
    this->normals_array.Bind();
    this->normals_buffer = VBO(&this->normals, this->normals.size() * sizeof(float));

    this->shader = Shader(vertex_shader_filename, fragment_shader_filename);

    this->vertices_array.link_vbo(this->vertices_buffer, 0, 3);
    this->normals_array.link_vbo(this->normals_buffer, 1, 3);

    this->point_light.position = point_light.position;
    this->point_light.constant = point_light.constant;
    this->point_light.linear = point_light.linear;
    this->point_light.quadratic = point_light.quadratic;
    this->point_light.ambient = point_light.ambient;
    this->point_light.diffuse = point_light.diffuse;
    this->point_light.specular = point_light.specular;

    this->position = glm::vec3(0.0, 0.0, 0.0);
}

void DrawableLight::Bind()
{
    this->vertices_array.Bind();
    this->normals_array.Bind();
}

void DrawableLight::Unbind()
{
    this->vertices_array.Unbind();
    this->normals_array.Unbind();
}

void DrawableLight::Draw()
{
    glDrawArrays(GL_TRIANGLE_FAN, 0,  vertices.size() / 3);
}

void DrawableLight::Draw(glm::mat4* model, glm::mat4* view, glm::mat4* projection, DRAWING_MODE drawing_mode, bool transparent, glm::vec3 camera_position)
{
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(*model));
    glUniformMatrix4fv(1, 1, GL_FALSE, glm::value_ptr(*view));
    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(*projection));

    // loadUniforms();

    if (transparent)
    {
        sortTriangles(camera_position);
        // this->vertices_buffer.Update(&this->vertices, this->vertices.size() * sizeof(float));
        // this->normals_buffer.Update(&this->normals, this->normals.size() * sizeof(float));
    
    }

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

void DrawableLight::loadUniforms()
{   
    glUniform3fv(glGetUniformLocation(shader.id, "centerPosition"), 1, glm::value_ptr(position));

    glUniform3fv(glGetUniformLocation(shader.id, "material.ambient"),  1, glm::value_ptr(material.ambient));
    glUniform3fv(glGetUniformLocation(shader.id, "material.diffuse"),  1, glm::value_ptr(material.diffuse));
    glUniform3fv(glGetUniformLocation(shader.id, "material.specular"), 1, glm::value_ptr(material.specular));
    glUniform1f (glGetUniformLocation(shader.id, "material.shininess"),   material.shininess);
}

void DrawableLight::loadData(const char* filename, std::vector<float> &data, float scale)
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

bool DrawableLight::loadFromObjectFile(const char* filename)
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


bool DrawableLight::replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

void DrawableLight::sortTriangles(glm::vec3 camera_position)
{
    std::vector<std::pair<glm::vec3, glm::vec3>> vert_norm_vector;

    for (int i = 0; i < this->vertices.size(); i += 3)
    {
        glm::vec3 vert(this->vertices[i], this->vertices[i + 1], this->vertices[i + 2]);
        glm::vec3 norm(this->normals[i], this->normals[i + 1], this->normals[i + 2]);
        std::pair<glm::vec3, glm::vec3> pair = {vert, norm};

        vert_norm_vector.push_back(pair);
    }

    

    std::sort(std::begin(vert_norm_vector), std::end(vert_norm_vector),
          [camera_position](const std::pair<glm::vec3, glm::vec3> &vert_a, const std::pair<glm::vec3, glm::vec3> &vert_b) {
            return glm::length(vert_a.first - camera_position) < glm::length(vert_b.first - camera_position);
          });

   
    int j = 0;
    for (int i = 0; i < vert_norm_vector.size(); i++)
    {
        this->vertices[j++]     = vert_norm_vector[i].first.x;
        // this->normals[j++]     = vert_norm_vector[i].second.x;

        this->vertices[j++] = vert_norm_vector[i].first.x;
        // this->normals[j++] = vert_norm_vector[i].second.y;

        this->vertices[j++] = vert_norm_vector[i].first.x;
        // this->normals[j++] = vert_norm_vector[i].second.z;

        // std::cout << "(" << p.first.x << ", " << p.first.y << ", " << p.first.z << ") (";
        // std::cout << "(" << p.second.x << ", " << p.second.y << ", " << p.second.z << ")\n";
    }

}