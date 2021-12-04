#include "includes.h"
#include "vao.h"
#include "vbo.h"
#include "shader.h"
#include "drawable.h"

class Player
{
    public:
        VAO vertices_array;
        VAO normals_array;
        
        VBO vertices_buffer;
        VBO normals_buffer;

        Shader shader;

        std::vector<float> vertices;
        // std::vector<glm::vec3> vertices;
        std::vector<float> normals;
        // std::vector<glm::vec3> normals;
        std::vector<float> data;

        glm::vec3 position;
        float speed;

    public:
        Player(const char* vertices_data_filename, const char* normals_data_filename, 
               const char* vertex_shader_filename, const char* fragment_shader_filename);
        Player(const char* obj_data_filename, const char* vertex_shader_filename,
               const char* fragment_shader_filename);
    public:
        void Bind();
        void Unbind();
        void Draw();
        void Draw(glm::mat4* model, glm::mat4* view, glm::mat4* projection, DRAWING_MODE drawing_mode);

    private:
        void loadData(const char* filename, std::vector<float> &data, float scale);
        bool loadFromObjectFile(const char* filename);
        bool replace(std::string& str, const std::string& from, const std::string& to);
};